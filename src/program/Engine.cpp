/**
 * @file program/Engine.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "program/Engine.hpp"

#include "data/Config.hpp"
#include "debug/Debug.hpp"
#include "debug/DebugConsole.hpp"
#include "debug/DebugManager.hpp"
#include "event/AppEvent.hpp"
#include "event/CoreEvents.hpp"
#include "event/CoreEventsData.hpp"
#include "event/EventManager.hpp"
#include "event/RuntimeEvent.hpp"
#include "mod/LuaBindings.hpp"
#include "mod/ModManager.hpp"
#include "mod/ScriptErrors.hpp"
#include "program/Application.hpp"
#include "program/Context.hpp"
#include "program/EngineComponent.hpp"
#include "program/EngineData.hpp"
#include "program/PrimaryWindow.hpp"
#include "program/Window.hpp"
#include "program/WindowManager.hpp"
#include "system/Log.hpp"
#include "system/LogMicros.hpp"
#include "util/MicrosImpl.hpp"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_timer.h"

#include <chrono>
#include <cmath>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

static constexpr bool DisableLoadingThread = false;

using namespace tudov;

Engine::Engine() noexcept
    : _log(Log::Get("Engine")),
      _state(EState::None),
      _beginTick(0),
      _previousTick(0),
      _framerate(0),
      _logicThread(),
      _loadingState(ELoadingState::Done),
      _loadingThread()
{
	_context = Context(this);

	_data = std::make_unique<EngineData>(_context);

	_loadingThread = std::thread(std::bind(&Engine::LoadingThread, this));
}

void Engine::LogicThread() noexcept
{
	//
}

void Engine::LoadingThread() noexcept
{
	LoadingInfoArgs args{
	    .progressTotal = 1.0f,
	};

	while (!DisableLoadingThread && !ShouldQuit())
	{
		if (_loadingMutex.try_lock())
		{
			if (_loadingState == ELoadingState::Pending)
			{
				_loadingState = ELoadingState::InProgress;
				_loadingMutex.unlock();

				TE_TRACE("Process background loading");

				args.title = "Background Loading";
				args.description = "Please wait.";
				args.progressValue = 0.0f;
				SetLoadingInfo(args);

				ProcessLoad();

				args.title = "Background Loaded";
				args.description = "Waiting to be ended.";
				args.progressValue = 1.0f;
				SetLoadingInfo(args);

				_loadingState = ELoadingState::Done;
			}
			else
			{
				_loadingMutex.unlock();
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	TE_DEBUG("{}", "Background loading thread exit");
}

Engine::~Engine() noexcept
{
	switch (_state)
	{
	case EState::Initialized:
	case EState::Quit:
		Deinitialize();
	default:
		break;
	}
}

bool Engine::ShouldQuit() noexcept
{
	switch (_state)
	{
	case EState::None:
		return false;
	case EState::Quit:
	case EState::Deinitialized:
		return true;
	case EState::Initialized:
		break;
	}

	return _data->_windowManager->IsEmpty();
}

Version Engine::GetAppVersion() const noexcept
{
	return Version(0, 1, 0);
}

void Engine::Initialize() noexcept
{
	TE_DEBUG("{}", "Initializing engine ...");
	{
		_data->_windowManager->InitializePrimaryWindow();

		for (auto it = _data->_components.begin(); it != _data->_components.end(); ++it)
		{
			(*it)->PreInitialize();
		}

		for (auto it = _data->_components.begin(); it != _data->_components.end(); ++it)
		{
			(*it)->Initialize();
		}

		PostInitialization();

		_state = EState::Initialized;
	}
	TE_DEBUG("{}", "Initialized engine");
}

void Engine::PostInitialization() noexcept
{
	ProvideDebug(*_data->_windowManager->GetDebugManager());

	_data->_modManager->LoadModsDeferred();
	_previousTick = SDL_GetTicksNS();
	_beginTick = SDL_GetTicksNS();
	_framerate = 0;
}

bool Engine::Tick() noexcept
{
	if (ShouldQuit())
	{
		return false;
	}

	std::uint64_t beginNS = SDL_GetTicksNS();
	_framerate = 1'000'000'000.0 / (beginNS - _previousTick);
	_previousTick = beginNS;

	if (_loadingState == ELoadingState::Done)
	{
		// recording background loading ticks.
		_loadingBeginNS = SDL_GetTicksNS();
	}

	if constexpr (DisableLoadingThread)
	{
		ProcessLoad();
	}
	else
	{
		// update loading info.
		if (_loadingInfo.progressVisualValue >= _loadingInfo.progressValue)
		{
			_loadingInfo.progressVisualValue = _loadingInfo.progressValue;
		}
		else
		{
			std::float_t factor = 1.0f - std::exp(-GetDeltaTime() / 0.1f);
			_loadingInfo.progressVisualValue = std::lerp(_loadingInfo.progressVisualValue, _loadingInfo.progressValue, factor);
		}
	}

	{
		std::lock_guard<std::timed_mutex> guard{_loadingMutex};
		if (_loadingState == ELoadingState::Done)
		{
			ProcessTick();
		}
	}
	ProcessRender();

	{
		std::uint64_t endNS = SDL_GetTicksNS();
		std::uint64_t limit = 1'000'000'000ull / uint64_t(Tudov::GetConfig().GetWindowFramelimit());
		std::uint64_t elapsed = endNS - beginNS;
		if (elapsed < limit)
		{
			SDL_DelayPrecise(limit - elapsed);
		}
	}

	return true;
}

void Engine::ProcessLoad() noexcept
{
	_data->_modManager->Update();
	_data->_eventManager->GetCoreEvents().TickLoad().Invoke();
	ProvideDebug(*_data->_windowManager->GetDebugManager());
}

void Engine::ProcessTick() noexcept
{
	if (!_data->_scriptErrors->HasLoadtimeError())
	{
		_data->_eventManager->GetCoreEvents().TickUpdate().Invoke();
	}

	for (const std::unique_ptr<AppEvent> &event : _appEvents)
	{
		HandleEvent(*event);
	}
	_appEvents.clear();
	_appEvents.reserve(0);
}

void Engine::HandleEvent(AppEvent &appEvent) noexcept
{
	for (std::shared_ptr<IEngineComponent> &component : _data->_components)
	{
		if (component->HandleEvent(appEvent))
		{
			return;
		}
	}
}

void Engine::ProcessRender() noexcept
{
	_data->_windowManager->Render();
}

void Engine::Event(AppEvent &appEvent) noexcept
{
	if (appEvent.sdlEvent->type == SDL_EVENT_QUIT) [[unlikely]]
	{
		Quit();
		return;
	}

	if (_appEvents.size() < 999) [[likely]]
	{
		_appEvents.emplace_back(std::make_unique<AppEvent>(*appEvent.sdlEvent));
	}
}

void Engine::Deinitialize() noexcept
{
	if (_state != EState::Quit)
	{
		return;
	}

	std::lock_guard<std::timed_mutex> guard{_loadingMutex};

	if (_loadingThread.joinable())
	{
		_loadingThread.join();
	}

	TE_DEBUG("{}", "Deinitializing engine ...");
	{
		PreDeinitialization();

		for (auto it = _data->_components.rbegin(); it != _data->_components.rend(); ++it)
		{
			(*it)->Deinitialize();
		}

		for (auto it = _data->_components.rbegin(); it != _data->_components.rend(); ++it)
		{
			(*it)->PostDeinitialize();
		}

		_state = EState::Deinitialized;
	}
	TE_DEBUG("{}", "Deinitialized engine");
}

void Engine::PreDeinitialization() noexcept
{
	_data->_modManager->UnloadMods();
}

void Engine::Quit()
{
	if (_state == EState::Initialized)
	{
		TE_DEBUG("{}", "Engine is pending quit!");

		_state = EState::Quit;
		_data->_windowManager->CloseWindows();
	}
}

void Engine::InitializePrimaryWindow() noexcept
{
}

void Engine::InitializeResources() noexcept
{
}

Context &Engine::GetContext() noexcept
{
	return _context;
}

Log &Engine::GetLog() noexcept
{
	return *_log;
}

void Engine::ProvideDebug(IDebugManager &debugManager) noexcept
{
	{
		EventDebugProvideData data{_context, debugManager};
		_data->_eventManager->GetCoreEvents().DebugProvide().Invoke(&data, {}, EEventInvocation::None);
	}

	if (auto &&debugConsole = debugManager.GetElement<DebugConsole>(); debugConsole != nullptr)
	{
		auto &&quit = [this](std::string_view)
		{
			this->Quit();

			return std::vector<DebugConsole::Result>();
		};

		debugConsole->SetCommand({
		    .name = "quit",
		    .help = "quit: Shutdown engine.",
		    .func = quit,
		});
	}

	for (auto &&component : _data->_components)
	{
		if (auto &&debugProvider = std::dynamic_pointer_cast<IDebugProvider>(component); debugProvider != nullptr)
		{
			debugProvider->ProvideDebug(debugManager);
		}
	}
}

std::float_t Engine::GetFramerate() const noexcept
{
	return _framerate;
}

std::float_t Engine::GetDeltaTime() const noexcept
{
	return 1 / _framerate;
}

std::uint64_t Engine::GetBeginTick() const noexcept
{
	return SDL_GetTicksNS();
}

std::uint64_t Engine::GetTick() const noexcept
{
	return SDL_GetTicksNS();
}

Engine::ELoadingState Engine::GetLoadingState() const noexcept
{
	return _loadingState;
}

std::timed_mutex &Engine::GetLoadingMutex() noexcept
{
	return _loadingMutex;
}

bool Engine::IsLoadingLagged() noexcept
{
	return !DisableLoadingThread && _loadingState == ELoadingState::InProgress;
}

std::uint64_t Engine::GetLoadingBeginTick() const noexcept
{
	return _loadingBeginNS;
}

Engine::LoadingInfo Engine::GetLoadingInfo() noexcept
{
	return _loadingInfo;
}

void Engine::SetLoadingInfo(const LoadingInfoArgs &loadingInfo) noexcept
{
	if (_loadingState == ELoadingState::InProgress)
	{
		_loadingInfo.title = loadingInfo.title.has_value() ? loadingInfo.title.value() : _loadingInfo.title;
		_loadingInfo.description = loadingInfo.description.has_value() ? loadingInfo.description.value() : _loadingInfo.description;
		_loadingInfo.progressValue = loadingInfo.progressValue.has_value() ? loadingInfo.progressValue.value() : _loadingInfo.progressValue;
		_loadingInfo.progressTotal = loadingInfo.progressTotal.has_value() ? loadingInfo.progressTotal.value() : _loadingInfo.progressTotal;
	}
}

void Engine::TriggerLoadPending() noexcept
{
	if (_loadingMutex.try_lock())
	{
		if (_loadingState == ELoadingState::Done)
		{
			_loadingState = ELoadingState::Pending;
		}
		_loadingMutex.unlock();
	}
}

std::shared_ptr<Version> Engine::LuaGetVersion() const noexcept
{
	return std::make_shared<Version>(GetAppVersion());
}

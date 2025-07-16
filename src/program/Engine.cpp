#include "Engine.hpp"

#include "Context.hpp"
#include "EngineComponent.hpp"
#include "MainWindow.hpp"
#include "Window.hpp"
#include "data/Config.hpp"
#include "debug/Debug.hpp"
#include "debug/DebugManager.hpp"
#include "mod/LuaAPI.hpp"
#include "mod/ModManager.hpp"
#include "network/Network.hpp"
#include "resource/FontManager.hpp"
#include "resource/ImageManager.hpp"
#include "resource/ResourceType.hpp"
#include "scripts/GameScripts.hpp"
#include "util/Log.hpp"
#include "util/MicrosImpl.hpp"
#include "util/StringUtils.hpp"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_timer.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <exception>
#include <memory>
#include <mutex>
#include <regex>
#include <thread>
#include <vector>

static constexpr bool DebugSingleThread = false;

using namespace tudov;

Engine::Engine(const MainArgs &args) noexcept
    : _log(Log::Get("Engine")),
      _state(EState::None),
      _config(std::make_shared<Config>()),
      _imageManager(std::make_shared<ImageManager>()),
      _fontManager(std::make_shared<FontManager>()),
      _luaAPI(std::make_shared<LuaAPI>()),
      _mainWindow(),
      _windows(),
      _debugManager(std::make_shared<DebugManager>()),
      mainArgs(args)
{
	context = Context(this);

	_network = std::make_shared<Network>(context);
	_modManager = std::make_shared<ModManager>(context);
	_scriptProvider = std::make_shared<ScriptProvider>(context);
	_scriptLoader = std::make_shared<ScriptLoader>(context);
	_scriptEngine = std::make_shared<ScriptEngine>(context);
	_eventManager = std::make_shared<EventManager>(context);
	_gameScripts = std::make_shared<GameScripts>(context);

	_loadingState = ELoadingState::Done;
	_loadingThread = std::thread([this]()
	{
		while (!DebugSingleThread && !ShouldQuit())
		{
			if (!_loadingMutex.try_lock())
			{
				continue;
			}
			_loadingMutex.unlock();

			if (ELoadingState expected = ELoadingState::Pending; _loadingState.compare_exchange_strong(expected, ELoadingState::InProgress))
			{
				SetLoadingInfo(Engine::LoadingInfoArgs{
				    .title = "Loading",
				    .description = "Please wait.",
				    .progressValue = 0.0f,
				    .progressTotal = 1.0f,
				});

				_modManager->Update();
				_eventManager->GetCoreEvents().TickLoad().Invoke();

				SetLoadingInfo(Engine::LoadingInfoArgs{
				    .title = "Loaded",
				    .description = "Waiting to be ended.",
				    .progressValue = 1.0f,
				    .progressTotal = 1.0f,
				});

				_loadingState.store(ELoadingState::Done, std::memory_order_release);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});
}

Engine::~Engine() noexcept
{
	if (_loadingThread.joinable())
	{
		_loadingThread.join();
	}

	switch (_state)
	{
	case EState::Initialized:
	case EState::Quit:
		Deinitialize();
	default:
		break;
	}

	Application::~Application();
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
	return _windows.empty();
}

void Engine::Initialize() noexcept
{
	_components = {
	    _modManager,
	    _network,
	    _eventManager,
	    _gameScripts,
	    _scriptProvider,
	    _scriptLoader,
	    _scriptEngine,
	};

	_log->Debug("Initializing engine ...");
	{
		_config->Load();
		InitializeMainWindow();
		InitializeResources();

		// ProvideLuaAPI(*_luaAPI);

		for (auto &&it = _components.begin(); it != _components.end(); ++it)
		{
			it->get()->Initialize();
		}

		ProvideDebug(*_debugManager);

		_modManager->LoadModsDeferred();
		_previousTime = SDL_GetTicksNS();
		_framerate = 0;
		_state = EState::Initialized;
	}
	_log->Debug("Initialized engine");
}

bool IsWindowShouldClose(const std::shared_ptr<IWindow> &window) noexcept
{
	return window->ShouldClose();
}

bool Engine::Tick() noexcept
{
	if (ShouldQuit())
	{
		return false;
	}

	std::uint64_t beginNS = SDL_GetTicksNS();
	_framerate = 1'000'000'000.0 / (beginNS - _previousTime);
	_previousTime = beginNS;

	if (ELoadingState expected = ELoadingState::Done; _loadingState.compare_exchange_strong(expected, ELoadingState::Pending))
	{
		// recording background loading ticks.
		_loadingBeginNS = SDL_GetTicksNS();
	}

	if (DebugSingleThread)
	{
		_modManager->Update();
		_eventManager->GetCoreEvents().TickLoad().Invoke();
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
		std::lock_guard<std::mutex> lock{_loadingMutex};

		if (_loadingState.load() != ELoadingState::InProgress)
		{
			if (!_scriptLoader->HasAnyLoadError())
			{
				_eventManager->GetCoreEvents().TickUpdate().Invoke();
			}

			for (auto &&event : _sdlEvents)
			{
				HandleEvent(*event);
				delete event;
			}
			_sdlEvents.clear();

			for (auto &&window : _windows)
			{
				window->HandleEvents();
			}

			_windows.erase(std::remove_if(_windows.begin(), _windows.end(), IsWindowShouldClose), _windows.end());
		}

		for (auto &&window : _windows)
		{
			window->Render();
		}
	}

	std::uint64_t endNS = SDL_GetTicksNS();
	auto limit = 1'000'000'000ull / uint64_t(_config->GetWindowFramelimit());
	auto elapsed = endNS - beginNS;
	if (elapsed < limit)
	{
		SDL_DelayPrecise(limit - elapsed);
	}

	return true;
}

void Engine::HandleEvent(SDL_Event &event) noexcept
{
	for (auto &&window : _windows)
	{
		window->HandleEvent(event);
	}
}

void Engine::Event(SDL_Event &event) noexcept
{
	switch (event.type)
	{
	case SDL_EVENT_QUIT:
		Quit();
		return;
	}

	_sdlEvents.emplace_back(new SDL_Event(event));
}

void Engine::Deinitialize() noexcept
{
	if (_state != EState::Quit)
	{
		return;
	}

	_log->Debug("Deinitializing engine ...");
	{
		_modManager->UnloadMods();

		for (auto &&it = _components.rbegin(); it != _components.rend(); ++it)
		{
			it->get()->Deinitialize();
		}

		_state = EState::Deinitialized;
	}
	_log->Debug("Deinitialized engine");
}

void Engine::Quit()
{
	if (_state == EState::Initialized)
	{
		_log->Debug("Engine is pending quit!");

		_state = EState::Quit;
		for (auto &&window : _windows)
		{
			window->Close();
		}
	}
}

void Engine::InitializeMainWindow() noexcept
{
	if (!_mainWindow.expired())
	{
		_log->Error("Engine main window has already been initialized!");
	}

	auto &&mainWindow = std::make_shared<MainWindow>(context);
	mainWindow->Initialize(_config->GetWindowWidth(), _config->GetWindowHeight(), _config->GetWindowTitle());
	AddWindow(mainWindow);
	_mainWindow = mainWindow;

	_debugManager = std::make_shared<DebugManager>();
	mainWindow->SetDebugManager(_debugManager);
}

void Engine::InitializeResources() noexcept
{
	_log->Debug("Mounting resource files");

	auto &&renderBackend = _config->GetRenderBackend();

	// auto &&loadTexture = [this, renderBackend]() {
	// 	switch (renderBackend) {
	// 		textureManager.Load<>(file, window.renderer, std::string_view(data))
	// 	}
	// };

	std::unordered_map<ResourceType, std::uint32_t> fileCounts{};

	std::vector<std::regex> mountBitmapPatterns{};
	for (auto &&pattern : _config->GetMountBitmaps())
	{
		mountBitmapPatterns.emplace_back(std::regex(std::string(pattern), std::regex_constants::icase));
	}

	auto &&mountDirectories = _config->GetMountFiles();
	for (auto &&mountDirectory : _config->GetMountDirectories())
	{
		if (!std::filesystem::exists(mountDirectory.data()) || !std::filesystem::is_directory(mountDirectory.data()))
		{
			_log->Warn("Invalid directory for mounting resources: {}", mountDirectory.data());
			continue;
		}

		for (auto &entry : std::filesystem::recursive_directory_iterator(mountDirectory))
		{
			if (!entry.is_regular_file())
			{
				continue;
			}

			auto &&path = entry.path();
			auto &&it = mountDirectories.find(path.extension().string());
			if (it == mountDirectories.end())
			{
				continue;
			}

			auto &&filePath = path.generic_string();
			auto imageID = _imageManager->Load(filePath);
			if (!imageID)
			{
				_log->Error("Image ID of \"{}\" is 0!", filePath);
				continue;
			}

			auto &&image = _imageManager->GetResource(imageID);
			auto &&fileMemory = ReadFileToString(filePath, true);
			image->Initialize(fileMemory);
			if (image->IsValid())
			{
				fileCounts.try_emplace(it->second, 0).first->second++;
			}

			// bitmap suppoert
			// auto &&relative = std::filesystem::relative(path, mountDirectory).generic_string();
			// for (auto &&pattern : mountBitmapPatterns)
			// {
			// 	if (std::regex_match(relative, pattern))
			// 	{
			// 		fontManager.AddBitmapFont(std::make_shared<BitmapFont>(textureID, std::string_view(data)));
			// 		break;
			// 	}
			// }
		}
	}

	_log->Debug("Mounted all resource files");
	for (auto [fileType, count] : fileCounts)
	{
		_log->Info("{}: {}", ResourceTypeToStringView(fileType).data(), count);
	}
}

void Engine::ProvideDebug(IDebugManager &debugManager) noexcept
{
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

	for (auto &&component : _components)
	{
		if (auto &&debugProvider = std::dynamic_pointer_cast<IDebugProvider>(component); debugProvider != nullptr)
		{
			debugProvider->ProvideDebug(*_debugManager);
		}
	}
}

// void Engine::ProvideLuaAPI(ILuaAPI &luaAPI) noexcept
// {
// 	_luaAPI->RegisterInstallation("tudov_Engine", [this](sol::state &lua)
// 	{
// 		auto GetMainWindow = [this]()
// 		{
// 			return _mainWindow.lock().get();
// 		};

// 		lua.new_usertype<Engine>("tudov_Engine",
// 		                         "mainWindow", sol::readonly_property(GetMainWindow),
// 		                         "quit", &Engine::Quit);
// 		lua["engine"] = this;
// 	});

// 	for (auto &&component : _components)
// 	{
// 		if (auto &&luaAPIProvider = std::dynamic_pointer_cast<ILuaAPIProvider>(component); luaAPIProvider != nullptr)
// 		{
// 			luaAPIProvider->ProvideLuaAPI(*_luaAPI);
// 		}
// 	}

// 	if (!_mainWindow.expired())
// 	{
// 		if (auto &&luaAPIProvider = std::dynamic_pointer_cast<ILuaAPIProvider>(_mainWindow.lock()); luaAPIProvider != nullptr)
// 		{
// 			luaAPIProvider->ProvideLuaAPI(luaAPI);
// 		}
// 	}
// }

template <typename T>
void Engine_ChangeEngineComponent(std::shared_ptr<Log> &log, std::string_view component, std::shared_ptr<T> &oldComponent, const std::shared_ptr<T> &newComponent) noexcept
{
	try
	{
		oldComponent->Deinitialize();
		newComponent->Initialize();
		oldComponent = newComponent;
	}
	catch (std::exception &e)
	{
		log->Error("Failed to change component `{}`: {}", component.data(), e.what());
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

std::uint64_t Engine::GetTick() const noexcept
{
	return SDL_GetTicksNS();
}

TE_GEN_GETTER_SMART_PTR(std::shared_ptr, IWindow, Engine::GetMainWindow, _mainWindow, noexcept);

void Engine::AddWindow(const std::shared_ptr<IWindow> &window)
{
	_windows.emplace_back(window);
}

void Engine::RemoveWindow(const std::shared_ptr<IWindow> &window)
{
	// TODO
}

Engine::ELoadingState Engine::GetLoadingState() noexcept
{
	return _loadingState.load();
}

bool Engine::IsLoadingLagged() noexcept
{
	return (SDL_GetTicksNS() - _loadingBeginNS) > (1'000'000'000ull / uint64_t(_config->GetWindowFramelimit() / 10));
}

std::uint64_t Engine::GetLoadingBeginTick() const noexcept
{
	return _loadingBeginNS;
}

Engine::LoadingInfo Engine::GetLoadingInfo() noexcept
{
	std::lock_guard<std::mutex> lock{_loadingInfoMutex};
	return _loadingInfo;
}

void Engine::SetLoadingInfo(LoadingInfoArgs loadingInfo) noexcept
{
	if (_loadingState.load() == ELoadingState::InProgress)
	{
		_loadingInfoMutex.lock();
		_loadingInfo.title = loadingInfo.title.has_value() ? loadingInfo.title.value() : _loadingInfo.title;
		_loadingInfo.description = loadingInfo.description.has_value() ? loadingInfo.description.value() : _loadingInfo.description;
		_loadingInfo.progressValue = loadingInfo.progressValue.has_value() ? loadingInfo.progressValue.value() : _loadingInfo.progressValue;
		_loadingInfo.progressTotal = loadingInfo.progressTotal.has_value() ? loadingInfo.progressTotal.value() : _loadingInfo.progressTotal;
		_loadingInfoMutex.unlock();
	}
}

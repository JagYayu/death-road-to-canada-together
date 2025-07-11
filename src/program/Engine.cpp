#include "Engine.hpp"

#include "Context.hpp"
#include "EngineComponent.hpp"
#include "MainWindow.hpp"
#include "SDL3/SDL_events.h"
#include "Window.hpp"
#include "debug/Debug.hpp"
#include "debug/DebugManager.hpp"
#include "mod/LuaAPI.hpp"
#include "network/Network.hpp"
#include "resource/ResourceType.hpp"
#include "scripts/GameScripts.hpp"
#include "sol/property.hpp"
#include "util/Log.hpp"
#include "util/MicrosImpl.hpp"
#include "util/StringUtils.hpp"

#include "SDL3/SDL_timer.h"

#include <algorithm>
#include <exception>
#include <memory>
#include <regex>
#include <vector>

using namespace tudov;

Engine::Engine(const MainArgs &args) noexcept
    : _log(Log::Get("Engine")),
      _state(EState::None),
      _config(),
      _imageManager(),
      _fontManager(),
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

	Application::~Application();
}

bool IsWindowShouldClose(const std::shared_ptr<IWindow> &window) noexcept
{
	return window->ShouldClose();
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
		_config.Load();
		InitializeMainWindow();
		InitializeResources();

		for (auto &&it = _components.begin(); it != _components.end(); ++it)
		{
			it->get()->Initialize();
		}

		ProvideDebug(*_debugManager);
		ProvideLuaAPI(*_luaAPI);

		_modManager->LoadMods();
		_previousTime = SDL_GetTicksNS();
		_framerate = 0;
		_state = EState::Initialized;
	}
	_log->Debug("Initialized engine");
}

bool Engine::Tick() noexcept
{
	switch (_state)
	{
	case EState::None:
		return true;
	case EState::Quit:
	case EState::Deinitialized:
		return false;
	case EState::Initialized:
		break;
	}

	if (_windows.empty())
	{
		return false;
	}

	uint64_t startNS = SDL_GetTicksNS();
	_framerate = 1'000'000'000.0 / (startNS - _previousTime);
	_previousTime = startNS;

	_modManager->Update();

	if (!_scriptLoader->HasAnyLoadError())
	{
		_eventManager->GetCoreEvents().TickUpdate()->Invoke();
	}

	for (auto &&event : _events)
	{
		HandleEvent(*event);
		delete event;
	}
	_events.clear();

	for (auto &&window : _windows)
	{
		window->HandleEvents();
	}

	if (!_mainWindow.expired())
	{
		_debugManager->UpdateAndRender(_mainWindow.lock());
	}

	for (auto &&window : _windows)
	{
		window->Render();
	}

	_windows.erase(std::remove_if(_windows.begin(), _windows.end(), IsWindowShouldClose), _windows.end());

	auto elapsed = SDL_GetTicksNS() - startNS;
	auto limit = 1'000'000'000ull / uint64_t(_config.GetWindowFramelimit());
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

	_events.emplace_back(new SDL_Event(event));
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
	AddWindow(mainWindow);
	mainWindow->Initialize(_config.GetWindowWidth(), _config.GetWindowHeight(), _config.GetWindowTitle());

	_mainWindow = mainWindow;
}

void Engine::InitializeResources() noexcept
{
	_log->Debug("Mounting resource files");

	auto &&renderBackend = _config.GetRenderBackend();

	// auto &&loadTexture = [this, renderBackend]() {
	// 	switch (renderBackend) {
	// 		textureManager.Load<>(file, window.renderer, std::string_view(data))
	// 	}
	// };

	std::unordered_map<ResourceType, std::uint32_t> fileCounts{};

	std::vector<std::regex> mountBitmapPatterns{};
	for (auto &&pattern : _config.GetMountBitmaps())
	{
		mountBitmapPatterns.emplace_back(std::regex(std::string(pattern), std::regex_constants::icase));
	}

	auto &&mountDirectories = _config.GetMountFiles();
	for (auto &&mountDirectory : _config.GetMountDirectories())
	{
		if (!std::filesystem::exists(mountDirectory) || !std::filesystem::is_directory(mountDirectory))
		{
			_log->Warn("Invalid directory for mounting resources: {}", mountDirectory);
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
			auto imageID = _imageManager.Load(filePath);
			if (!imageID)
			{
				_log->Error("Image ID of \"{}\" is 0!", filePath);
				continue;
			}

			auto &&image = _imageManager.GetResource(imageID);
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
		_log->Info("{}: {}", ResourceTypeToStringView(fileType), count);
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

void Engine::ProvideLuaAPI(ILuaAPI &luaAPI) noexcept
{
	static constexpr auto classname = "tudov_Engine";

	_luaAPI->RegisterInstallation(classname, [this](sol::state &lua)
	{
		auto GetMainWindow = [this]()
		{
			return _mainWindow.lock().get();
		};

		lua.new_usertype<Engine>(classname,
		                         "mainWindow", sol::readonly_property(GetMainWindow),
		                         "quit", &Engine::Quit);
		lua["engine"] = this;
	});

	for (auto &&component : _components)
	{
		if (auto &&luaAPIProvider = std::dynamic_pointer_cast<ILuaAPIProvider>(component); luaAPIProvider != nullptr)
		{
			luaAPIProvider->ProvideLuaAPI(*_luaAPI);
		}
	}
}

std::float_t Engine::GetFramerate() const noexcept
{
	return _framerate;
}

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
		log->Error("Failed to change component `{}`: {}", component, e.what());
	}
}

void Engine::ChangeModManager(const std::shared_ptr<IModManager> &modManager) noexcept
{
	Engine_ChangeEngineComponent(_log, "IModManager", _modManager, modManager);
}

void Engine::ChangeScriptEngine(const std::shared_ptr<IScriptEngine> &scriptEngine) noexcept
{
	Engine_ChangeEngineComponent(_log, "IScriptEngine", _scriptEngine, scriptEngine);
}

void Engine::ChangeScriptLoader(const std::shared_ptr<IScriptLoader> &scriptLoader) noexcept
{
	Engine_ChangeEngineComponent(_log, "IScriptLoader", _scriptLoader, scriptLoader);
}

void Engine::ChangeScriptProvider(const std::shared_ptr<IScriptProvider> &scriptProvider) noexcept
{
	Engine_ChangeEngineComponent(_log, "IScriptProvider", _scriptProvider, scriptProvider);
}

void Engine::ChangeEventManager(const std::shared_ptr<IEventManager> &eventManager) noexcept
{
	Engine_ChangeEngineComponent(_log, "IEventManager", _eventManager, eventManager);
}

void Engine::ChangeGameScripts(const std::shared_ptr<IGameScripts> &gameScripts) noexcept
{
	Engine_ChangeEngineComponent(_log, "IGameScripts", _gameScripts, gameScripts);
}

TUDOV_GEN_GETTER_SMART_PTR(std::shared_ptr, IWindow, Engine::GetMainWindow, _mainWindow, noexcept);

void Engine::AddWindow(const std::shared_ptr<IWindow> &window)
{
	_windows.emplace_back(window);
}

void Engine::RemoveWindow(const std::shared_ptr<IWindow> &window)
{
	// TODO
}

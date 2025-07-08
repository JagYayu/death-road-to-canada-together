#include "Engine.h"

#include "Context.h"
#include "EngineComponent.h"
#include "SDL3/SDL_timer.h"
#include "debug/DebugManager.h"
#include "mod/LuaAPI.h"
#include "resource/ResourceType.hpp"
#include "scripts/GameScripts.h"
#include "util/Log.h"
#include "util/MicrosImpl.h"
#include "util/StringUtils.hpp"

#include <algorithm>
#include <exception>
#include <memory>
#include <regex>
#include <stdexcept>
#include <vector>

using namespace tudov;

Engine::Engine(const MainArgs &args) noexcept
    : _log(Log::Get("Engine")),
      _running(true),
      _framerate(0),
      _config(),
      _imageManager(),
      _fontManager(),
      _luaAPI(std::make_shared<LuaAPI>()),
      _mainWindow(),
      _windows(),
      _debugManager(),
      mainArgs(args)
{
	context = Context(this);

	_modManager = std::make_shared<ModManager>(context);
	_scriptProvider = std::make_shared<ScriptProvider>(context);
	_scriptLoader = std::make_shared<ScriptLoader>(context);
	_scriptEngine = std::make_shared<ScriptEngine>(context);
	_eventManager = std::make_shared<EventManager>(context);
	_gameScripts = std::make_shared<GameScripts>(context);
}

Engine::~Engine() noexcept
{
}

bool IsWindowShouldClose(const std::shared_ptr<IWindow> &window) noexcept
{
	return window->ShouldClose();
}

void Engine::Run()
{
	std::vector<std::shared_ptr<IEngineComponent>> engineComponents{
	    _modManager,
	    _scriptProvider,
	    _scriptLoader,
	    _scriptEngine,
	    _eventManager,
	    _gameScripts,
	};

	_log->Debug("Initializing engine ...");
	{
		_config.Load();
		InitializeMainWindow();
		InitializeResources();

		for (auto &&it = engineComponents.begin(); it != engineComponents.end(); ++it)
		{
			it->get()->Initialize();
		}

		_modManager->LoadMods();
	}
	_log->Debug("Initialized engine");

	std::uint64_t prevNS = SDL_GetTicksNS();

	while (_running && !_windows.empty())
	{
		uint64_t startNS = SDL_GetTicksNS();
		uint64_t deltaNS = startNS - prevNS;
		prevNS = startNS;
		_framerate = 1'000'000'000.0 / deltaNS;

		_modManager->Update();

		for (auto &&window : _windows)
		{
			window->HandleEvents();
		}

		_debugManager->UpdateAndRender();

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
	}

	_log->Debug("Deinitializing engine ...");
	{
		_modManager->UnloadMods();

		for (auto &&it = engineComponents.rbegin(); it != engineComponents.rend(); ++it)
		{
			it->get()->Deinitialize();
		}
	}
	_log->Debug("Deinitialized engine");
}

void Engine::Quit()
{
	_running = false;
	for (auto &&window : _windows)
	{
		window->Close();
	}
}

void Engine::InitializeMainWindow()
{
	if (!_mainWindow.expired())
	{
		throw std::runtime_error("Engine main window has already been initialized!");
	}

	auto &&mainWindow = std::make_shared<MainWindow>(context);
	AddWindow(mainWindow);
	mainWindow->Initialize(_config.GetWindowWidth(), _config.GetWindowHeight(), _config.GetWindowTitle());

	_mainWindow = mainWindow;
	_debugManager = std::make_shared<DebugManager>(mainWindow);
}

void Engine::InitializeResources()
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
	_windows.emplace_back(window); // TODO
}

void Engine::RemoveWindow(const std::shared_ptr<IWindow> &window)
{
	// TODO
}

std::shared_ptr<IWindow> Engine::LuaGetMainWindow() noexcept
{
	return _mainWindow.expired() ? nullptr : _mainWindow.lock();
}

void InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept
{
	auto &&table = scriptEngine.CreateTable();

	//

	scriptEngine.SetReadonlyGlobal("Engine", table);
}

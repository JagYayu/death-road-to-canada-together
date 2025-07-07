#include "Engine.h"

#include "Context.h"
#include "SDL3/SDL_timer.h"
#include "debug/DebugManager.h"
#include "mod/LuaAPI.h"
#include "resource/ResourceType.hpp"
#include "util/Log.h"
#include "util/MicrosImpl.h"
#include "util/StringUtils.hpp"

#include <algorithm>
#include <memory>
#include <regex>
#include <stdexcept>
#include <vector>

using namespace tudov;

Engine::Engine() noexcept
    : _log(Log::Get("Engine")),
      _running(true),
      _framerate(0),
      _config(),
      _imageManager(),
      _fontManager(),
      _luaAPI(std::make_shared<LuaAPI>()),
      _mainWindow(nullptr),
      _windows()
{
	context = Context(this);

	_modManager = std::make_shared<ModManager>(context);
	_scriptProvider = std::make_shared<ScriptProvider>(context);
	_scriptLoader = std::make_shared<ScriptLoader>(context);
	_scriptEngine = std::make_shared<ScriptEngine>(context);
	_eventManager = std::make_shared<EventManager>(context);
}

Engine::~Engine() noexcept
{
}

bool IsWindowShouldClose(const std::shared_ptr<IWindow> &window) noexcept
{
	return window->ShouldClose();
}

void Engine::Run(const MainArgs &args)
{
	_log->Debug("Initializing engine ...");
	{
		_config.Load();
		InitializeMainWindow();
		InitializeResources();
		_modManager->Initialize();
		_scriptProvider->Initialize();
		_scriptLoader->Initialize();
		_scriptEngine->Initialize();
		_eventManager->Initialize();
		// InstallToScriptEngine(_modManager.scriptEngine);
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

		for (auto &&window : _windows)
		{
			window->HandleEvents();
		}

		_modManager->Update();

		_debugManager->UpdateAndRender();

		for (auto &&window : _windows)
		{
			window->Render();
		}

		_windows.erase(std::remove_if(_windows.begin(), _windows.end(), IsWindowShouldClose), _windows.end());

		uint64_t elapsed = SDL_GetTicksNS() - startNS;
		const uint64_t limit = 1'000'000'000ull / static_cast<uint64_t>(_config.GetWindowFramelimit());
		if (elapsed < limit)
		{
			SDL_DelayPrecise(limit - elapsed);
		}
	}

	_log->Debug("Deinitializing engine ...");
	{
		_modManager->UnloadMods();
		_eventManager->Deinitialize();
		_scriptEngine->Deinitialize();
		_scriptLoader->Deinitialize();
		_scriptProvider->Deinitialize();
		_modManager->Deinitialize();
	}
	_log->Debug("Deinitialized engine");
}

void Engine::Quit()
{
	for (auto &&window : _windows)
	{
		window->Close();
	}
	_running = false;
}

void Engine::InitializeMainWindow()
{
	if (_mainWindow)
	{
		throw std::runtime_error("Engine main window has already been initialized!");
	}

	_mainWindow = std::make_shared<MainWindow>(context);
	AddWindow(_mainWindow);
	_mainWindow->Initialize(_config.GetWindowWidth(), _config.GetWindowHeight(), _config.GetWindowTitle());

	_debugManager = std::make_shared<DebugManager>(_mainWindow);
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
	return _mainWindow;
}

void InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept
{
	auto &&table = scriptEngine.CreateTable();

	//

	scriptEngine.SetReadonlyGlobal("Engine", table);
}

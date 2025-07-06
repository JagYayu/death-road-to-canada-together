#include "Engine.h"

#include "SDL3/SDL_timer.h"
#include "debug/DebugManager.h"
#include "resource/ResourceType.hpp"
#include "util/Log.h"
#include "util/StringUtils.hpp"

#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include <memory>
#include <regex>
#include <stdexcept>
#include <vector>

using namespace tudov;

Engine::Engine() noexcept
    : _log(Log::Get("Engine")),
      _running(true),
      modManager(*this),
      imageManager(),
      fontManager(),
      mainWindow(nullptr)
{
}

Engine::~Engine() noexcept
{
	if (mainWindow)
	{
		ImGui_ImplSDLRenderer3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
	}
}

void Engine::Run(const MainArgs &args)
{
	_log->Debug("Initializing engine ...");
	{
		config.Load();
		InitializeMainWindow();
		InitializeResources();
		modManager.Initialize();
		InstallToScriptEngine(modManager.scriptEngine);
		modManager.LoadMods();
	}
	_log->Debug("Initialized engine");

	std::uint64_t prevNS = SDL_GetTicksNS();

	while (_running)
	{
		uint64_t startNS = SDL_GetTicksNS();
		uint64_t deltaNS = startNS - prevNS;
		prevNS = startNS;
		_framerate = 1'000'000'000.0 / deltaNS;

		for (auto &&window : _windows)
		{
			window->HandleEvents();
		}

		modManager.Update();

		debugManager->UpdateAndRender();

		for (auto &&window : _windows)
		{
			window->Render();
		}

		uint64_t elapsed = SDL_GetTicksNS() - startNS;
		const uint64_t limit = 1'000'000'000ull / static_cast<uint64_t>(config.GetWindowFramelimit());
		if (elapsed < limit)
		{
			SDL_DelayPrecise(limit - elapsed);
		}
	}

	_log->Debug("Deinitializing engine ...");
	{
		modManager.Deinitialize();
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
	if (mainWindow)
	{
		throw std::runtime_error("Engine main window has already been initialized!");
	}

	mainWindow = std::make_shared<MainWindow>(*this);
	AddWindow(mainWindow);
	mainWindow->Initialize(config.GetWindowWidth(), config.GetWindowHeight(), config.GetWindowTitle());

	debugManager = std::make_shared<DebugManager>(mainWindow);
}

void Engine::InitializeResources()
{
	_log->Debug("Mounting resource files");

	auto &&renderBackend = config.GetRenderBackend();

	// auto &&loadTexture = [this, renderBackend]() {
	// 	switch (renderBackend) {
	// 		textureManager.Load<>(file, window.renderer, std::string_view(data))
	// 	}
	// };

	std::unordered_map<ResourceType, std::uint32_t> fileCounts{};

	std::vector<std::regex> mountBitmapPatterns{};
	for (auto &&pattern : config.GetMountBitmaps())
	{
		mountBitmapPatterns.emplace_back(std::regex(std::string(pattern), std::regex_constants::icase));
	}

	auto &&mountDirectories = config.GetMountFiles();
	for (auto &&mountDirectory : config.GetMountDirectories())
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
			auto imageID = imageManager.Load(filePath);
			if (!imageID)
			{
				_log->Error("Image ID of \"{}\" is 0!", filePath);
				continue;
			}

			auto &&image = imageManager.GetResource(imageID);
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

void Engine::AddWindow(const std::shared_ptr<Window> &window)
{
	_windows.emplace_back(window);
}

std::shared_ptr<Window> Engine::LuaGetMainWindow() noexcept
{
	return mainWindow;
}

void InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept
{
	auto &&table = scriptEngine.CreateTable();

	//

	scriptEngine.SetReadonlyGlobal("Engine", table);
}

#include "Engine.h"

#include "graphic/DynamicBuffer.h"
#include "graphic/RenderBuffer.h"
#include "graphic/Windows.h"
#include "graphic/sdl/SDLTexture.h"
#include "resource/ResourceType.hpp"
#include "util/Log.h"
#include "util/StringUtils.hpp"

#include <SDL3/SDL_system.h>
#include <bgfx/bgfx.h>
#include <bgfx/defines.h>
#include <bgfx/platform.h>

#include <memory>
#include <regex>
#include <vector>

using namespace tudov;

Engine::Engine() noexcept
    : _log(Log::Get("Engine")),
      _running(true),
      _bgfxInitialized(false),
      windows(*this),
      modManager(*this),
      texture2DManager(),
      fontManager()
{
}

Engine::~Engine() noexcept
{
	if (_bgfxInitialized)
	{
		bgfx::shutdown();
	}
}

inline bgfx::PlatformData GetPlatformData(Window &window)
{
	SDL_PropertiesID props = SDL_GetWindowProperties(window.GetHandle());
	bgfx::PlatformData pd{};

#if defined(SDL_PLATFORM_WIN32)
	pd.nwh = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
	pd.type = bgfx::NativeWindowHandleType::Default;
#elif defined(SDL_PLATFORM_MACOS)
	// TODO
#elif defined(SDL_PLATFORM_LINUX)
	// TODO
#endif

	return pd;
}

void Engine::InitializeBGFXWindow(Window &window) noexcept
{
	bgfx::Init init{};
	init.type = bgfx::RendererType::Count;
	init.vendorId = BGFX_PCI_ID_NONE;
	init.platformData = GetPlatformData(window);
	init.resolution.width = 800;
	init.resolution.height = 600;
	init.resolution.reset = BGFX_RESET_VSYNC;

	if (!bgfx::init(init))
	{
		_log->Error("Failed to initialize bgfx");
	}
	_bgfxInitialized = true;
}

void Engine::SwitchWindow(Window &window)
{
	bgfx::setPlatformData(GetPlatformData(window));

	auto &&size = window.GetSize();
	bgfx::reset(std::get<0>(size), std::get<1>(size), BGFX_RESET_VSYNC);
}

void Engine::Run(const MainArgs &args)
{
	SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);

	_log->Debug("Initializing engine ...");
	{
		config.Load();
		auto &&mainWindow = windows.CreateWindow("", config.GetWindowTitle(), config.GetWindowWidth(), config.GetWindowHeight());
		mainWindow->Initialize();
		InitializeBGFXWindow(*mainWindow);
		InitializeResources();
		modManager.Initialize();
		modManager.LoadMods();
	}
	_log->Debug("Initialized engine");

	std::shared_ptr<DynamicBuffer> buffer;

	while (_running)
	{
		// if (!bgfx::renderFrame(10))
		// {
		// 	_log->Error("bgfx::renderFrame error");
		// }

		for (auto &&window : windows)
		{
			window->PollEvents();
		}

		modManager.Update();

		auto firstWindow = true;

		for (auto &&window : windows)
		{
			if (firstWindow)
			{
				buffer = std::make_shared<DynamicBuffer>(window->renderer);
				firstWindow = false;
			}

			buffer->Clear();
			DynamicBuffer::Draw2DArgs args{};
			args.tex = 1;
			args.src = {0, 0, 24, 24};
			args.dst = {0, 0, 800, 800};
			buffer->Draw2D(args);
			buffer->Render();

			SwitchWindow(*window);
			window->Render();
		}

		bgfx::frame();
	}

	_log->Debug("Deinitializing engine ...");
	{
		modManager.Deinitialize();
	}
	_log->Debug("Deinitialized engine");
}

void Engine::Quit()
{
	_running = false;
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
			if (it != mountDirectories.end())
			{
				auto &&filePath = path.generic_string();
				auto &&data = ReadFileToString(filePath, true);

				// auto &&sdlRenderer = dynamic_cast<SDLRenderer *>(window.renderer.get());
				// auto &&sdlSurface = std::make_shared<SDLSurface>(*sdlRenderer, std::string_view(data));
				// TextureID textureID = texture2DManager.Load<Texture2D>(filePath, *(sdlSurface->GetRaw()));
				// if (textureID)
				// {
				// 	fileCounts.try_emplace(it->second, 0).first->second++;
				// }
				// else
				// {
				// 	_log->Error("Texture ID of \"{}\" is 0!", filePath);
				// }

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
	}

	_log->Debug("Mounted all resource files");
	for (auto [fileType, count] : fileCounts)
	{
		_log->Info("{}: {}", ResourceTypeToStringView(fileType), count);
	}
}

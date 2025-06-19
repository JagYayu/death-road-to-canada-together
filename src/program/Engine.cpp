#include "Engine.h"

#include "graphic/ERenderBackend.h"
#include "graphic/sdl/SDLRenderer.h"
#include "graphic/sdl/SDLSurface.h"
#include "graphic/sdl/SDLTexture.h"
#include "resource/ResourceType.hpp"
#include "util/Log.h"
#include "util/StringUtils.hpp"

#include <memory>
#include <regex>
#include <vector>

using namespace tudov;

Engine::Engine()
    : _log(Log::Get("Engine")),
      window(*this),
      modManager(*this),
      textureManager(),
      fontManager()
{
}

void Engine::Run(const MainArgs &args)
{
	SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);

	_log->Debug("Initializing engine ...");
	{
		config.Load();
		window.Initialize();
		InitializeResources();
		modManager.Initialize();
		modManager.LoadMods();
	}
	_log->Debug("Initialized engine");

	while (_running)
	{
		window.PoolEvents();
		modManager.Update();
		window.Render();
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
				auto &&file = path.string();
				auto &&data = ReadFileToString(file, true);

				TextureID textureID;

				switch (window.renderer->GetRenderBackend())
				{
				case ERenderBackend::SDL:
				{
					auto &&sdlRenderer = dynamic_cast<SDLRenderer *>(window.renderer.get());
					auto &&sdlSurface = std::make_shared<SDLSurface>(*sdlRenderer, std::string_view(data));
					textureID = textureManager.Load<SDLTexture>(file, *sdlRenderer, sdlSurface);
					if (textureID)
					{
						fileCounts.try_emplace(it->second, 0).first->second++;
						_log->Trace("{}", file);
					}
				}
				default:
					break;
				}

				auto &&relative = std::filesystem::relative(path, mountDirectory).generic_string();
				for (auto &&pattern : mountBitmapPatterns)
				{
					if (std::regex_match(relative, pattern))
					{
						fontManager.SetBitmapFont(textureID, BitmapFont(std::string_view(data)));
						break;
					}
				}
			}
		}
	}

	_log->Debug("Mounted all resource files");
	for (auto [fileType, count] : fileCounts)
	{
		_log->Info("{}: {}", ResourceTypeToStringView(fileType), count);
	}
}

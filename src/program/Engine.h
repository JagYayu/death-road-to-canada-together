#pragma once

#include "EngineConfig.h"
#include "MainArgs.hpp"
#include "debug/DebugManager.h"
#include "graphic/ERenderBackend.h"
#include "graphic/Windows.h"
#include "mod/ModManager.h"
#include "resource/BitmapFontManager.hpp"
#include "resource/FontManager.hpp"
#include "resource/ShaderManager.hpp"
#include "resource/Texture2DManager.hpp"
#include "util/Defs.h"

#include <map>
#include <memory>
#include <vector>

#undef CreateWindow

namespace tudov
{
	class Engine
	{
	  private:
		std::shared_ptr<Log> _log;
		bool _running;
		bool _bgfxInitialized;
		std::map<std::string, std::shared_ptr<Window>> _windows;

	  public:
		EngineConfig config;
		Texture2DManager texture2DManager;
		ShaderManager shaderManager;
		FontManager fontManager;
		// BitmapFontManager bitmapFontManager;
		Windows windows;
		ModManager modManager;

	  public:
		explicit Engine() noexcept;
		~Engine() noexcept;

	  private:
	  void InitializeBGFXWindow(Window&window)noexcept;
		void InitializeResources();

	  public:
		void SwitchWindow(Window &window);

		void Run(const MainArgs &args);
		void Quit();
	};
} // namespace tudov

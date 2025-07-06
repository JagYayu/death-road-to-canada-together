#pragma once

#include "EngineConfig.h"
#include "MainArgs.hpp"
#include "debug/DebugManager.h"
#include "graphic/Renderer.h"
#include "graphic/MainWindow.h"
#include "mod/ModManager.h"
#include "resource/FontManager.hpp"
#include "resource/ShaderManager.hpp"
#include "resource/ImageManager.hpp"
#include "util/Defs.h"

#include <map>
#include <memory>
#include <vector>

namespace tudov
{
	class Engine
	{
	  private:
		std::shared_ptr<Log> _log;
		bool _running;
		std::vector<std::shared_ptr<Window>> _windows;
		std::float_t _framerate;

	  public:
		EngineConfig config;
		ImageManager imageManager;
		// ShaderManager shaderManager;
		FontManager fontManager;
		ModManager modManager;
		std::shared_ptr<MainWindow> mainWindow;
		std::shared_ptr<DebugManager> debugManager;

	  public:
		explicit Engine() noexcept;
		~Engine() noexcept;

	  private:
		void InitializeMainWindow();
		void InitializeResources();

		std::shared_ptr<Window> LuaGetMainWindow() noexcept;

	  public:
		std::float_t GetFramerate() const noexcept;

		void AddWindow(const std::shared_ptr<Window> &window);
		void RemoveWindow(const std::shared_ptr<Window> &window);

		void Run(const MainArgs &args);
		void Quit();

		void InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept{}
	};
} // namespace tudov

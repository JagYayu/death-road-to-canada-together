#pragma once

#include "Context.h"
#include "EngineConfig.h"
#include "MainArgs.hpp"
#include "debug/DebugManager.h"
#include "graphic/MainWindow.h"
#include "graphic/Renderer.h"
#include "mod/LuaAPI.h"
#include "mod/ModManager.h"
#include "resource/FontManager.hpp"
#include "resource/ImageManager.hpp"
#include "resource/ShaderManager.hpp"
#include "util/Defs.h"


#include <map>
#include <memory>
#include <vector>

namespace tudov
{
	class Context;

	class Engine
	{
		friend Context;

	  private:
		std::shared_ptr<Log> _log;
		bool _running;
		std::float_t _framerate;

		EngineConfig _config;
		ImageManager _imageManager;
		// ShaderManager shaderManager;
		FontManager _fontManager;

		std::shared_ptr<ILuaAPI> _luaAPI;
		std::shared_ptr<IModManager> _modManager;
		std::shared_ptr<IScriptEngine> _scriptEngine;
		std::shared_ptr<IScriptLoader> _scriptLoader;
		std::shared_ptr<IScriptProvider> _scriptProvider;
		std::shared_ptr<IEventManager> _eventManager;

		std::weak_ptr<IWindow> _mainWindow;
		std::vector<std::shared_ptr<IWindow>> _windows;
		std::shared_ptr<DebugManager> _debugManager;

	  public:
		Context context;

	  public:
		explicit Engine() noexcept;
		~Engine() noexcept;

	  private:
		void InitializeMainWindow();
		void InitializeResources();

		std::shared_ptr<IWindow> LuaGetMainWindow() noexcept;

	  public:
		std::float_t GetFramerate() const noexcept;
		
		std::shared_ptr<IWindow> GetMainWindow() noexcept;
		std::shared_ptr<const IWindow> GetMainWindow()const noexcept;
		void AddWindow(const std::shared_ptr<IWindow> &window);
		void RemoveWindow(const std::shared_ptr<IWindow> &window);

		void Run(const MainArgs &args);
		void Quit();
	};
} // namespace tudov

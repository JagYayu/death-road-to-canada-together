#pragma once

#include "Context.hpp"
#include "EngineConfig.hpp"
#include "MainArgs.hpp"
#include "MainWindow.hpp"
#include "Window.hpp"
#include "debug/DebugManager.hpp"
#include "mod/LuaAPI.hpp"
#include "mod/ModManager.hpp"
#include "resource/FontManager.hpp"
#include "resource/ImageManager.hpp"
#include "resource/ShaderManager.hpp"
#include "scripts/GameScripts.hpp"

#include <memory>
#include <queue>
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
		std::float_t _previousTime;
		std::float_t _framerate;
		std::vector<std::shared_ptr<IEngineComponent>> _components;
		std::vector<SDL_Event *> _events;

		EngineConfig _config;
		ImageManager _imageManager;
		// ShaderManager shaderManager;
		FontManager _fontManager;

		std::shared_ptr<ILuaAPI> _luaAPI;
		std::shared_ptr<IModManager> _modManager;
		std::shared_ptr<IEventManager> _eventManager;
		std::shared_ptr<IGameScripts> _gameScripts;
		std::shared_ptr<IScriptEngine> _scriptEngine;
		std::shared_ptr<IScriptLoader> _scriptLoader;
		std::shared_ptr<IScriptProvider> _scriptProvider;

		std::weak_ptr<IWindow> _mainWindow;
		std::vector<std::shared_ptr<IWindow>> _windows;
		std::shared_ptr<DebugManager> _debugManager;

	  public:
		MainArgs mainArgs;
		Context context;

	  public:
		explicit Engine(const MainArgs &args = MainArgs()) noexcept;
		~Engine() noexcept;

	  private:
		void InitializeMainWindow() noexcept;
		void InitializeResources() noexcept;
		void HandleEvent(SDL_Event &event) noexcept;

		std::shared_ptr<IWindow> LuaGetMainWindow() noexcept;

	  public:
		std::float_t GetFramerate() const noexcept;

		// These functions are not fully tested yet, can lead to errors.
		void ChangeModManager(const std::shared_ptr<IModManager> &modManager) noexcept;
		void ChangeScriptEngine(const std::shared_ptr<IScriptEngine> &scriptEngine) noexcept;
		void ChangeScriptLoader(const std::shared_ptr<IScriptLoader> &scriptLoader) noexcept;
		void ChangeScriptProvider(const std::shared_ptr<IScriptProvider> &scriptProvider) noexcept;
		void ChangeEventManager(const std::shared_ptr<IEventManager> &eventManager) noexcept;
		void ChangeGameScripts(const std::shared_ptr<IGameScripts> &gameScripts) noexcept;

		std::shared_ptr<IWindow> GetMainWindow() noexcept;
		std::shared_ptr<const IWindow> GetMainWindow() const noexcept;
		void AddWindow(const std::shared_ptr<IWindow> &window);
		void RemoveWindow(const std::shared_ptr<IWindow> &window);

		void Initialize() noexcept;
		bool Tick() noexcept;
		void Event(SDL_Event &event) noexcept;
		void Deinitialize() noexcept;

		void Quit();
	};
} // namespace tudov

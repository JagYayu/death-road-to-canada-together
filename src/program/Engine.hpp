#pragma once

#include "Context.hpp"
#include "MainArgs.hpp"
#include "Window.hpp"
#include "data/Config.hpp"
#include "debug/DebugManager.hpp"
#include "mod/LuaAPI.hpp"
#include "mod/ModManager.hpp"
#include "resource/FontManager.hpp"
#include "resource/ImageManager.hpp"
#include "resource/ShaderManager.hpp"
#include "scripts/GameScripts.hpp"

#include <memory>
#include <vector>

namespace tudov
{
	struct Application
	{
		virtual ~Application() noexcept = default;

		virtual void Initialize() noexcept = 0;
		virtual bool Tick() noexcept = 0;
		virtual void Event(SDL_Event &event) noexcept = 0;
		virtual void Deinitialize() noexcept = 0;
	};

	class Context;

	class Engine : public Application
	{
		friend Context;

	  private:
		enum class EState
		{
			None,
			Initialized,
			Quit,
			Deinitialized,
		};

	  private:
		std::shared_ptr<Log> _log;
		EState _state;
		std::float_t _previousTime;
		std::float_t _framerate;
		std::vector<std::shared_ptr<IEngineComponent>> _components;
		std::vector<SDL_Event *> _events;

		Config _config;
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
		~Engine() noexcept override;

	  private:
		void InitializeMainWindow() noexcept;
		void InitializeResources() noexcept;
		void HandleEvent(SDL_Event &event) noexcept;

		std::shared_ptr<IWindow> LuaGetMainWindow() noexcept;

	  public:
		void Initialize() noexcept override;
		bool Tick() noexcept override;
		void Event(SDL_Event &event) noexcept override;
		void Deinitialize() noexcept override;

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

		void Quit();
	};
} // namespace tudov

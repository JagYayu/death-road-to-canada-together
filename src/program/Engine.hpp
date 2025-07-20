#pragma once

#include "Context.hpp"
#include "MainArgs.hpp"
#include "debug/Debug.hpp"
#include "program/EngineComponent.hpp"

#include <atomic>
#include <cmath>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

union SDL_Event;

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
	class Log;
	struct IGameScripts;
	struct ILuaAPI;
	struct INetwork;
	struct IStorageManager;
	struct IWindow;

	class Engine : public Application, public IDebugProvider
	{
		friend Context;

	  public:
		enum class ELoadingState
		{
			Done,
			Pending,
			InProgress,
		};

		struct LoadingInfoArgs
		{
			std::optional<std::string> title = std::nullopt;
			std::optional<std::string> description = std::nullopt;
			std::optional<std::float_t> progressValue = std::nullopt;
			std::optional<std::float_t> progressTotal = std::nullopt;
		};

	  private:
		enum class EState
		{
			None,
			Initialized,
			Quit,
			Deinitialized,
		};

		struct LoadingInfo
		{
			friend Engine;

		  private:
			std::float_t progressVisualValue = 0.0f;

		  public:
			std::string title;
			std::string description;
			std::float_t progressValue = 0.0f;
			std::float_t progressTotal = 0.0f;

			constexpr std::float_t GetVisualProgressValue() const noexcept
			{
				return progressVisualValue;
			}

			constexpr std::float_t GetVisualProgress() const noexcept
			{
				return progressVisualValue / progressTotal;
			}
		};

	  private:
		std::shared_ptr<Log> _log;
		EState _state;
		std::float_t _previousTime;
		std::float_t _framerate;
		std::vector<std::shared_ptr<IEngineComponent>> _components;
		std::vector<SDL_Event *> _sdlEvents;
		std::atomic<ELoadingState> _loadingState;
		std::thread _loadingThread;
		std::mutex _loadingMutex;
		std::uint64_t _loadingBeginNS;
		LoadingInfo _loadingInfo;
		std::mutex _loadingInfoMutex;

		std::shared_ptr<Config> _config;
		std::shared_ptr<ImageManager> _imageManager;
		// ShaderManager shaderManager;
		std::shared_ptr<FontManager> _fontManager;

		std::shared_ptr<ILuaAPI> _luaAPI;
		std::shared_ptr<IStorageManager> _storageManager;
		std::shared_ptr<INetwork> _network;
		std::shared_ptr<IModManager> _modManager;
		std::shared_ptr<IEventManager> _eventManager;
		std::shared_ptr<IGameScripts> _gameScripts;
		std::shared_ptr<IScriptEngine> _scriptEngine;
		std::shared_ptr<IScriptLoader> _scriptLoader;
		std::shared_ptr<IScriptProvider> _scriptProvider;

		std::weak_ptr<IWindow> _mainWindow;
		std::vector<std::shared_ptr<IWindow>> _windows;
		std::shared_ptr<IDebugManager> _debugManager;

	  public:
		MainArgs mainArgs;
		Context context;

	  public:
		explicit Engine(const MainArgs &args = MainArgs()) noexcept;
		~Engine() noexcept override;

	  private:
		bool ShouldQuit() noexcept;
		void InitializeMainWindow() noexcept;
		void InitializeResources() noexcept;
		void HandleEvent(SDL_Event &event) noexcept;

	  public:
		void Initialize() noexcept override;
		bool Tick() noexcept override;
		void Event(SDL_Event &event) noexcept override;
		void Deinitialize() noexcept override;

		void ProvideDebug(IDebugManager &debugManager) noexcept override;
		// void ProvideLuaAPI(ILuaAPI &luaAPI) noexcept override;

		// Basic operations.

		std::float_t GetFramerate() const noexcept;
		std::float_t GetDeltaTime() const noexcept;
		std::uint64_t GetTick() const noexcept;
		void Quit();

		// Windows operations.

		std::shared_ptr<IWindow> GetMainWindow() noexcept;
		std::shared_ptr<const IWindow> GetMainWindow() const noexcept;
		void AddWindow(const std::shared_ptr<IWindow> &window);
		void RemoveWindow(const std::shared_ptr<IWindow> &window);

		// Background loading info operations.

		bool IsLoadingLagged() noexcept;
		ELoadingState GetLoadingState() noexcept;
		std::uint64_t GetLoadingBeginTick() const noexcept;
		LoadingInfo GetLoadingInfo() noexcept;
		void SetLoadingInfo(const LoadingInfoArgs &loadingInfo) noexcept;

		// Inline implementations.

		inline const LoadingInfo GetLoadingInfo() const noexcept
		{
			return const_cast<Engine *>(this)->GetLoadingInfo();
		}

		inline Engine &ChangeLoadingProgress(std::float_t value) noexcept
		{
			auto &&info = GetLoadingInfo();
			SetLoadingInfo(LoadingInfoArgs{
			    .progressValue = info.progressValue + value,
			});
			return *this;
		}

		inline Engine &SetLoadingDescription(std::string_view description) noexcept
		{
			auto &&info = GetLoadingInfo();
			SetLoadingInfo(LoadingInfoArgs{
			    .description = std::string(description),
			});
			return *this;
		}
	};
} // namespace tudov

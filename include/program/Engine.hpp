#pragma once

#include "Application.hpp"
#include "Context.hpp"
#include "debug/Debug.hpp"
#include "program/EngineComponent.hpp"
#include "util/Log.hpp"

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
	struct IGameScripts;
	struct ILuaAPI;
	struct INetworkManager;
	struct IGlobalStorageManager;
	struct IGraphicDevice;
	struct IAssetsManager;
	struct IWindow;
	class Context;
	class Log;
	class GlobalResourcesCollection;
	class VirtualFileSystem;

	/**
	 * @brief Tudov game engine
	 */
	class Engine : public Application, public IDebugProvider, public ILogProvider
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

		  private:
			std::float_t progressVisualValue = 0.0f;
		};

	  private:
		EState _state;
		std::uint64_t _beginTick;
		std::float_t _previousTick;
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
		std::shared_ptr<Log> _log;
		std::shared_ptr<ILuaAPI> _luaAPI;
		std::shared_ptr<IGlobalStorageManager> _globalStorageManager;
		std::shared_ptr<VirtualFileSystem> _virtualFileSystem;

		std::shared_ptr<GlobalResourcesCollection> _globalResourcesCollection;
		std::shared_ptr<IAssetsManager> _assetsManager;
		std::shared_ptr<INetworkManager> _networkManager;
		std::shared_ptr<IModManager> _modManager;
		std::shared_ptr<IEventManager> _eventManager;
		std::shared_ptr<IGameScripts> _gameScripts;
		std::shared_ptr<IScriptEngine> _scriptEngine;
		std::shared_ptr<IScriptLoader> _scriptLoader;
		std::shared_ptr<IScriptProvider> _scriptProvider;

		// std::shared_ptr<FontResources> _fontResources;
		// std::shared_ptr<ImageResources> _imageResources;
		// std::shared_ptr<TextResources> _textResources;
		// std::shared_ptr<ShaderManager> shaderResources;

		std::weak_ptr<IWindow> _mainWindow;
		std::vector<std::shared_ptr<IWindow>> _windows;
		std::shared_ptr<IDebugManager> _debugManager;

	  public:
		Context context;

	  public:
		explicit Engine() noexcept;
		~Engine() noexcept override;

	  private:
		void BackgroundLoadingThread() noexcept;
		bool ShouldQuit() noexcept;
		void InitializeMainWindow() noexcept;
		void InitializeResources() noexcept;
		void PostInitialization() noexcept;
		void HandleEvent(SDL_Event &event) noexcept;

	  public:
		void Initialize() noexcept override;
		bool Tick() noexcept override;
		void Event(void *event) noexcept override;
		void Deinitialize() noexcept override;

		Log &GetLog() noexcept override;
		void ProvideDebug(IDebugManager &debugManager) noexcept override;

		// Basic operations.

		std::float_t GetFramerate() const noexcept;
		std::float_t GetDeltaTime() const noexcept;
		std::uint64_t GetBeginTick() const noexcept;
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

		inline Engine &AddLoadingProgress(std::float_t value) noexcept
		{
			auto &&info = GetLoadingInfo();
			SetLoadingInfo(LoadingInfoArgs{.progressValue = info.progressValue + value});
			return *this;
		}

		inline Engine &SetLoadingDescription(std::string_view description) noexcept
		{
			SetLoadingInfo(LoadingInfoArgs{.description = std::string(description)});
			return *this;
		}
	};
} // namespace tudov

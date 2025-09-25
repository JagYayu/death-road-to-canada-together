/**
 * @file program/Engine.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Application.hpp"
#include "Context.hpp"
#include "debug/Debug.hpp"
#include "program/Context.hpp"
#include "program/EngineComponent.hpp"
#include "system/Log.hpp"

#include <cmath>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

namespace tudov
{
	class EngineData;

	class Context;
	class Log;
	class LuaBindings;

	/**
	 * @brief Tudov game engine
	 */
	class Engine final : public Application, public IContextProvider, public IDebugProvider, private ILogProvider
	{
		friend Context;
		friend EngineData;
		friend LuaBindings;

	  public:
		enum class ELoadingState : std::uint8_t
		{
			// Load done, set by loading thread.
			Done,
			// Load done, set by main thread.
			Pending,
			// Loading thread is loading in progress.
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
		std::vector<std::unique_ptr<AppEvent>> _appEvents;
		std::unique_ptr<EngineData> _data;

		// Background loading thread
		std::thread _logicThread;
		ELoadingState _loadingState;
		std::thread _loadingThread;
		std::timed_mutex _loadingMutex;
		std::uint64_t _loadingBeginNS;
		LoadingInfo _loadingInfo;

		std::shared_ptr<Log> _log;

		Context _context;

	  public:
		explicit Engine() noexcept;
		~Engine() noexcept override;

	  public:
		Version GetAppVersion() const noexcept override;
		void Initialize() noexcept override;
		bool Tick() noexcept override;
		void Event(AppEvent &appEvent) noexcept override;
		void Deinitialize() noexcept override;

		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;
		void ProvideDebug(IDebugManager &debugManager) noexcept override;

		// Basic operations.

		std::float_t GetFramerate() const noexcept;
		std::float_t GetDeltaTime() const noexcept;
		std::uint64_t GetBeginTick() const noexcept;
		std::uint64_t GetTick() const noexcept;
		void Quit();

		// Background loading info operations.

		bool IsLoadingLagged() noexcept;
		ELoadingState GetLoadingState() const noexcept;
		std::timed_mutex &GetLoadingMutex() noexcept;
		std::uint64_t GetLoadingBeginTick() const noexcept;
		LoadingInfo GetLoadingInfo() noexcept;
		void SetLoadingInfo(const LoadingInfoArgs &loadingInfo) noexcept;
		void TriggerLoadPending() noexcept;

	  private:
		void LoadingThread() noexcept;
		bool ShouldQuit() noexcept;

		void PostInitialization() noexcept;
		void PreDeinitialization() noexcept;
		void HandleEvent(AppEvent &appEvent) noexcept;

		void ProcessLoad() noexcept;
		void ProcessTick() noexcept;
		void ProcessRender() noexcept;

		[[deprecated]]
		void LogicThread() noexcept;
		[[deprecated]]
		void InitializePrimaryWindow() noexcept;
		[[deprecated]]
		void InitializeResources() noexcept;

	  public:
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

	  private:
		std::shared_ptr<Version> LuaGetVersion() const noexcept;
	};
} // namespace tudov

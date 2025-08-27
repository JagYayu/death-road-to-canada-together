/**
 * @file system/Log.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "LogVerbosity.hpp"
#include "program/Tudov.hpp"
#include "sol/table.hpp"
#include "util/Micros.hpp"

#include <atomic>
#include <chrono>
#include <mutex>
#include <queue>
#include <unordered_map>

namespace tudov
{
	class LuaAPI;

	/**
	 * Customized logging system for Tudov Engine.
	 * Cross platform is not supported.
	 */
	class Log
	{
		friend LuaAPI;

	  private:
		struct Entry
		{
			std::chrono::system_clock::time_point time;
			std::string_view verbosity;
			std::string module;
			std::string message;
		};

	  private:
		static constexpr decltype(auto) VerbTrace = "Trace";
		static constexpr decltype(auto) VerbInfo = "Info";
		static constexpr decltype(auto) VerbDebug = "Debug";
		static constexpr decltype(auto) VerbWarn = "Warn";
		static constexpr decltype(auto) VerbError = "Error";
		static constexpr decltype(auto) VerbFatal = "Fatal";

	  private:
		static ELogVerbosity _globalVerbosities;
		static std::unordered_map<std::string, ELogVerbosity> _verbosities;
		static std::unordered_map<std::string, ELogVerbosity> _verbositiesOverrides;
		static std::unordered_map<std::string, std::shared_ptr<Log>> _logInstances;
		static std::queue<Entry> _queue;
		static std::mutex _mutex;
		static std::condition_variable _cv;
		static std::atomic<bool> _exit;

	  private:
		static void Process() noexcept;
		static void PrintToConsole(std::string_view message, const Entry &entry) noexcept;
		static void OutputImpl(std::string_view module, std::string_view verb, std::string_view str);

	  public:
		static std::shared_ptr<Log> Get(std::string_view module) noexcept;

		static Log &GetInstance() noexcept;

		static void CleanupExpired() noexcept;

		/**
		 * Quit the logging system, will wait until logging thread has being finished, then call `Exit()` internally.
		 */
		static void Quit() noexcept;

		/**
		 * Call this function only before the program is about to exit.
		 * e.g. Abort the program.
		 * Otherwise, use `Exit()` instead.
		 */
		static void Exit() noexcept;

		static ELogVerbosity GetGlobalVerbosities() noexcept;

		static ELogVerbosity GetVerbosities(std::string_view module) noexcept;

		static bool SetVerbosities(std::string_view module, ELogVerbosity flags) noexcept;

		static void UpdateVerbosities(const void *jsonConfig);

		static std::optional<ELogVerbosity> GetVerbositiesOverride(const std::string &module) noexcept;

		static void SetVerbositiesOverride(std::string_view module, ELogVerbosity flags) noexcept;

		static bool RemoveVerbositiesOverride(std::string_view module) noexcept;

		static std::size_t CountLogs() noexcept;

		static std::unordered_map<std::string, std::shared_ptr<Log>>::const_iterator BeginLogs() noexcept;

		static std::unordered_map<std::string, std::shared_ptr<Log>>::const_iterator EndLogs() noexcept;

		static std::size_t CountVerbosities() noexcept;

		static std::unordered_map<std::string, ELogVerbosity>::const_iterator BeginVerbosities() noexcept;

		static std::unordered_map<std::string, ELogVerbosity>::const_iterator EndVerbosities() noexcept;

		static std::size_t CountVerbositiesOverrides() noexcept;

		static std::unordered_map<std::string, ELogVerbosity>::const_iterator BeginVerbositiesOverrides() noexcept;

		static std::unordered_map<std::string, ELogVerbosity>::const_iterator EndVerbositiesOverrides() noexcept;

	  private:
		std::string _module;

		[[nodiscard]] bool CanOutput(ELogVerbosity flag) const noexcept;
		void Output(std::string_view verb, std::string_view str) const noexcept;

	  public:
		explicit Log(std::string_view module) noexcept;
		~Log() noexcept;

		std::string_view GetModule() const;

		[[nodiscard]] TE_FORCEINLINE ELogVerbosity GetVerbosities() const noexcept
		{
			return GetVerbosities(_module);
		}

		[[nodiscard]] TE_FORCEINLINE bool CanTrace() const noexcept
		{
			return CanOutput(ELogVerbosity::Trace);
		}
		[[nodiscard]] TE_FORCEINLINE bool CanInfo() const noexcept
		{
			return CanOutput(ELogVerbosity::Info);
		}
		[[nodiscard]] TE_FORCEINLINE bool CanDebug() const noexcept
		{
			return CanOutput(ELogVerbosity::Debug);
		}
		[[nodiscard]] TE_FORCEINLINE bool CanWarn() const noexcept
		{
			return CanOutput(ELogVerbosity::Warn);
		}
		[[nodiscard]] TE_FORCEINLINE bool CanError() const noexcept
		{
			return CanOutput(ELogVerbosity::Error);
		}
		[[nodiscard]] TE_FORCEINLINE constexpr bool CanFatal() const noexcept
		{
			return true;
		}

		template <typename... TArgs>
		TE_FORCEINLINE void Trace(std::format_string<TArgs...> fmt, TArgs &&...args) const noexcept
		{
			Output(VerbTrace, std::format(fmt, std::forward<TArgs>(args)...));
		}
		template <typename... TArgs>
		TE_FORCEINLINE void Debug(std::format_string<TArgs...> fmt, TArgs &&...args) const noexcept
		{
			Output(VerbDebug, std::format(fmt, std::forward<TArgs>(args)...));
		}
		template <typename... TArgs>
		TE_FORCEINLINE void Info(std::format_string<TArgs...> fmt, TArgs &&...args) const noexcept
		{
			Output(VerbInfo, std::format(fmt, std::forward<TArgs>(args)...));
		}
		template <typename... TArgs>
		TE_FORCEINLINE void Warn(std::format_string<TArgs...> fmt, TArgs &&...args) const noexcept
		{
			Output(VerbWarn, std::format(fmt, std::forward<TArgs>(args)...));
		}
		template <typename... TArgs>
		TE_FORCEINLINE void Error(std::format_string<TArgs...> fmt, TArgs &&...args) const noexcept
		{
			Output(VerbError, std::format(fmt, std::forward<TArgs>(args)...));
		}
		/**
		 * @warning This function will terminate the program.
		 */
		template <typename... TArgs>
		[[noreturn]] TE_FORCEINLINE void Fatal(std::format_string<TArgs...> fmt, TArgs &&...args) const noexcept
		{
			auto &&str = std::format(fmt, std::forward<TArgs>(args)...);
			Output(VerbFatal, str);
			Tudov::FatalError(str);
		}

	  private:
		void LuaOutput(sol::object verb, sol::table args) const noexcept;
	};

	struct ILogProvider
	{
		virtual ~ILogProvider() noexcept = default;

		[[nodiscard]] virtual Log &GetLog() noexcept = 0;

		[[nodiscard]] const Log &GetLog() const noexcept
		{
			return const_cast<ILogProvider *>(this)->GetLog();
		}

		[[nodiscard]] const std::string &GetModule() const;

		[[nodiscard]] TE_FORCEINLINE ELogVerbosity GetVerbosity() const
		{
			return GetLog().GetVerbosities();
		}

		[[nodiscard]] TE_FORCEINLINE bool CanTrace() const noexcept
		{
			return GetLog().CanTrace();
		}
		[[nodiscard]] TE_FORCEINLINE bool CanInfo() const noexcept
		{
			return GetLog().CanInfo();
		}
		[[nodiscard]] TE_FORCEINLINE bool CanDebug() const noexcept
		{
			return GetLog().CanDebug();
		}
		[[nodiscard]] TE_FORCEINLINE bool CanWarn() const noexcept
		{
			return GetLog().CanWarn();
		}
		[[nodiscard]] TE_FORCEINLINE bool CanError() const noexcept
		{
			return GetLog().CanError();
		}
		[[nodiscard]] TE_FORCEINLINE constexpr bool CanFatal() const noexcept
		{
			return true;
		}

		template <typename... TArgs>
		TE_FORCEINLINE void Trace(std::format_string<TArgs...> fmt, TArgs &&...args) const
		{
			GetLog().Trace(fmt, std::forward<TArgs>(args)...);
		}
		template <typename... TArgs>
		TE_FORCEINLINE void Debug(std::format_string<TArgs...> fmt, TArgs &&...args) const
		{
			GetLog().Debug(fmt, std::forward<TArgs>(args)...);
		}
		template <typename... TArgs>
		TE_FORCEINLINE void Info(std::format_string<TArgs...> fmt, TArgs &&...args) const
		{
			GetLog().Info(fmt, std::forward<TArgs>(args)...);
		}
		template <typename... TArgs>
		TE_FORCEINLINE void Warn(std::format_string<TArgs...> fmt, TArgs &&...args) const
		{
			GetLog().Warn(fmt, std::forward<TArgs>(args)...);
		}
		template <typename... TArgs>
		TE_FORCEINLINE void Error(std::format_string<TArgs...> fmt, TArgs &&...args) const
		{
			GetLog().Error(fmt, std::forward<TArgs>(args)...);
		}
		template <typename... TArgs>
		[[noreturn]] TE_FORCEINLINE void Fatal(std::format_string<TArgs...> fmt, TArgs &&...args) const
		{
			GetLog().Fatal(fmt, std::forward<TArgs>(args)...);
		}
	};
} // namespace tudov

/**
 * @file System/Log.hpp
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
#include "Program/Tudov.hpp"
#include "Util/Micros.hpp"
#include "sol/table.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <vector>

namespace tudov
{
	class LuaBindings;

	/**
	 * Customized logging system for Tudov Engine.
	 * Cross platform is not supported.
	 */
	class Log
	{
		friend LuaBindings;

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
		static std::unordered_map<std::string, std::shared_ptr<Log>> _logInstanceMap;
		static std::unique_ptr<std::vector<Log *>> _logInstanceList;
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

		static void SaveVerbosities(void *jsonConfig);

		static void LoadVerbosities(const void *jsonConfig);

		static std::size_t CountLogs() noexcept;

		static std::vector<Log *>::const_iterator BeginLogs() noexcept;

		static std::vector<Log *>::const_iterator EndLogs() noexcept;

		static std::size_t CountVerbosities() noexcept;

		static std::unordered_map<std::string, ELogVerbosity>::const_iterator BeginVerbosities() noexcept;

		static std::unordered_map<std::string, ELogVerbosity>::const_iterator EndVerbosities() noexcept;

	  private:
		static std::vector<Log *> &GetOrNewLogInstanceList() noexcept;

	  private:
		std::string _module;

	  public:
		explicit Log(std::string_view module) noexcept;
		explicit Log(const Log &) noexcept = delete;
		explicit Log(Log &&) noexcept = delete;
		Log &operator=(const Log &) noexcept = delete;
		Log &operator=(Log &&) noexcept = delete;
		~Log() noexcept;

		std::string_view GetModule() const noexcept;

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
		[[nodiscard]] bool CanOutput(ELogVerbosity flag) const noexcept;
		void Output(std::string_view verb, std::string_view str) const noexcept;

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

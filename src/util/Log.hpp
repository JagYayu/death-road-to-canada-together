#pragma once

#include "Micros.hpp"

#include <json.hpp>
#include <sol/lua_value.hpp>

#include <atomic>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>

namespace tudov
{
	class Log
	{
	  private:
		struct Entry
		{
			std::chrono::system_clock::time_point time;
			std::string_view verbosity;
			std::string module;
			std::string message;
		};

	  public:
		enum class EVerbosity
		{
			All = -1,
			None = 0,
			Error = 1 << 0,
			Warn = 1 << 1,
			Info = 1 << 2,
			Debug = 1 << 3,
			Trace = 1 << 4,
			Fatal = 1 << 5,
		};

	  private:
		static constexpr decltype(auto) VerbTrace = "Trace";
		static constexpr decltype(auto) VerbInfo = "Info";
		static constexpr decltype(auto) VerbDebug = "Debug";
		static constexpr decltype(auto) VerbWarn = "Warn";
		static constexpr decltype(auto) VerbError = "Error";
		static constexpr decltype(auto) VerbFatal = "Fatal";

	  private:
		static EVerbosity _globalVerbosity;
		static std::unordered_map<std::string, EVerbosity> _moduleVerbs;
		static std::unordered_map<std::string, EVerbosity> _moduleVerbOverrides;
		static std::unordered_map<std::string, std::shared_ptr<Log>> _logInstances;
		static std::queue<Entry> _queue;
		static std::mutex _mutex;
		static std::condition_variable _cv;
		static std::atomic<bool> _exit;

	  private:
		static void Process();
		static void Output(std::string_view module, std::string_view verb, std::string_view str);

	  public:
		static std::shared_ptr<Log> Get(std::string_view module) noexcept;
		static Log &GetInstance() noexcept;
		static void CleanupExpired() noexcept;
		static void Quit() noexcept;
		static EVerbosity GetVerbosity(const std::string &module);
		static std::optional<EVerbosity> GetVerbosityOverride(const std::string &module);
		static void SetVerbosityOverride(const std::string &module, EVerbosity verb);
		static void UpdateVerbosities(const nlohmann::json &config);
		static void Exit() noexcept;

	  private:
		std::string _module;

		bool CanOutput(std::string_view verb) const;
		void Output(std::string_view verb, std::string_view str) const;

	  public:
		explicit Log(const std::string &module) noexcept;
		~Log() noexcept;

		const std::string &GetModule() const;

		TUDOV_FORCEINLINE EVerbosity GetVerbosity() const
		{
			return GetVerbosity(_module);
		}

		TUDOV_FORCEINLINE bool CanTrace() const
		{
			return CanOutput(VerbTrace);
		}
		TUDOV_FORCEINLINE bool CanInfo() const
		{
			return CanOutput(VerbInfo);
		}
		TUDOV_FORCEINLINE bool CanDebug() const
		{
			return CanOutput(VerbDebug);
		}
		TUDOV_FORCEINLINE bool CanWarn() const
		{
			return CanOutput(VerbWarn);
		}
		TUDOV_FORCEINLINE bool CanError() const
		{
			return CanOutput(VerbError);
		}
		TUDOV_FORCEINLINE bool CanFatal() const
		{
			return CanOutput(VerbFatal);
		}

		template <typename... Args>
		TUDOV_FORCEINLINE void Trace(std::format_string<Args...> fmt, Args &&...args)
		{
			Output(VerbTrace, std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		TUDOV_FORCEINLINE void Debug(std::format_string<Args...> fmt, Args &&...args)
		{
			Output(VerbDebug, std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		TUDOV_FORCEINLINE void Info(std::format_string<Args...> fmt, Args &&...args)
		{
			Output(VerbInfo, std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		TUDOV_FORCEINLINE void Warn(std::format_string<Args...> fmt, Args &&...args)
		{
			Output(VerbWarn, std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		TUDOV_FORCEINLINE void Error(std::format_string<Args...> fmt, Args &&...args)
		{
			Output(VerbError, std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		TUDOV_FORCEINLINE void Fatal(std::format_string<Args...> fmt, Args &&...args)
		{
			Output(VerbFatal, std::format(fmt, std::forward<Args>(args)...));
		}
	};

	struct ILogProvider
	{
		virtual ~ILogProvider() noexcept = default;

		virtual Log &GetLog() noexcept = 0;

		const Log &GetLog() const noexcept
		{
			return const_cast<ILogProvider *>(this)->GetLog();
		}

		const std::string &GetModule() const;

		TUDOV_FORCEINLINE Log::EVerbosity GetVerbosity() const
		{
			return GetLog().GetVerbosity();
		}

		TUDOV_FORCEINLINE bool CanTrace() const
		{
			return GetLog().CanTrace();
		}
		TUDOV_FORCEINLINE bool CanInfo() const
		{
			return GetLog().CanInfo();
		}
		TUDOV_FORCEINLINE bool CanDebug() const
		{
			return GetLog().CanDebug();
		}
		TUDOV_FORCEINLINE bool CanWarn() const
		{
			return GetLog().CanWarn();
		}
		TUDOV_FORCEINLINE bool CanError() const
		{
			return GetLog().CanError();
		}
		TUDOV_FORCEINLINE bool CanFatal() const
		{
			return GetLog().CanFatal();
		}

		template <typename... Args>
		TUDOV_FORCEINLINE void Trace(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Trace(fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		TUDOV_FORCEINLINE void Debug(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Debug(fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		TUDOV_FORCEINLINE void Info(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Info(fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		TUDOV_FORCEINLINE void Warn(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Warn(fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		TUDOV_FORCEINLINE void Error(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Error(fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		TUDOV_FORCEINLINE void Fatal(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Fatal(fmt, std::forward<Args>(args)...);
		}
	};
} // namespace tudov

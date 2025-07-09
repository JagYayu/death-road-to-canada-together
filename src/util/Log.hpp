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
		static EVerbosity _globalVerbosity;
		static std::unordered_map<std::string, EVerbosity> _moduleVerbs;
		static std::unordered_map<std::string, EVerbosity> _moduleVerbOverrides;
		static std::unordered_map<std::string, std::shared_ptr<Log>> _logInstances;
		static std::queue<Entry> _queue;
		static std::mutex _mutex;
		static std::condition_variable _cv;
		static std::atomic<bool> _exit;

	  public:
		static Log instance;

	  private:
		static void Process();

	  public:
		static std::shared_ptr<Log> Get(std::string_view module);
		static void CleanupExpired();
		static EVerbosity GetVerbosity(const std::string &module);
		static std::optional<EVerbosity> GetVerbosityOverride(const std::string &module);
		static void SetVerbosityOverride(const std::string &module, EVerbosity verb);
		static void UpdateVerbosities(const nlohmann::json &config);
		static void Exit() noexcept;

	  private:
		std::string _module;

		bool CanOutput(std::string_view verb) const;
		void Output(std::string_view verb, const std::string_view &str) const;

	  public:
		explicit Log(const std::string &module) noexcept;
		~Log() noexcept;

		const std::string &GetModule() const;

		FORCEINLINE EVerbosity GetVerbosity() const
		{
			return GetVerbosity(_module);
		}

		FORCEINLINE bool CanTrace() const
		{
			return CanOutput("Trace");
		}
		FORCEINLINE bool CanInfo() const
		{
			return CanOutput("Info");
		}
		FORCEINLINE bool CanDebug() const
		{
			return CanOutput("Debug");
		}
		FORCEINLINE bool CanWarn() const
		{
			return CanOutput("Warn");
		}
		FORCEINLINE bool CanError() const
		{
			return CanOutput("Error");
		}
		FORCEINLINE bool CanFatal() const
		{
			return CanOutput("Fatal");
		}

		template <typename... Args>
		FORCEINLINE void Trace(std::format_string<Args...> fmt, Args &&...args)
		{
			Output("Trace", std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		FORCEINLINE void Debug(std::format_string<Args...> fmt, Args &&...args)
		{
			Output("Debug", std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		FORCEINLINE void Info(std::format_string<Args...> fmt, Args &&...args)
		{
			Output("Info", std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		FORCEINLINE void Warn(std::format_string<Args...> fmt, Args &&...args)
		{
			Output("Warn", std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		FORCEINLINE void Error(std::format_string<Args...> fmt, Args &&...args)
		{
			Output("Error", std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		FORCEINLINE void Fatal(std::format_string<Args...> fmt, Args &&...args)
		{
			Output("Fatal", std::format(fmt, std::forward<Args>(args)...));
		}
	};

	struct ILogProvider
	{
		virtual Log &GetLog() noexcept = 0;

		const Log &GetLog() const noexcept
		{
			return const_cast<ILogProvider *>(this)->GetLog();
		}

		const std::string &GetModule() const;

		FORCEINLINE Log::EVerbosity GetVerbosity() const
		{
			return GetLog().GetVerbosity();
		}

		FORCEINLINE bool CanTrace() const
		{
			return GetLog().CanTrace();
		}
		FORCEINLINE bool CanInfo() const
		{
			return GetLog().CanInfo();
		}
		FORCEINLINE bool CanDebug() const
		{
			return GetLog().CanDebug();
		}
		FORCEINLINE bool CanWarn() const
		{
			return GetLog().CanWarn();
		}
		FORCEINLINE bool CanError() const
		{
			return GetLog().CanError();
		}
		FORCEINLINE bool CanFatal() const
		{
			return GetLog().CanFatal();
		}

		template <typename... Args>
		FORCEINLINE void Trace(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Trace(fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		FORCEINLINE void Debug(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Debug(fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		FORCEINLINE void Info(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Info(fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		FORCEINLINE void Warn(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Warn(fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		FORCEINLINE void Error(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Error(fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		FORCEINLINE void Fatal(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Fatal(fmt, std::forward<Args>(args)...);
		}
	};
} // namespace tudov

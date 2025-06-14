#pragma once

#include "Defs.h"

#include <json.hpp>
#include <sol/lua_value.hpp>

#include <atomic>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>

namespace tudov
{
	class Log
	{
	  private:
		struct Entry
		{
			std::chrono::system_clock::time_point time;
			StringView verbosity;
			String module;
			String message;
		};

	  public:
		enum class Verbosity
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
		static Verbosity _globalVerbosity;
		static UnorderedMap<String, Verbosity> _moduleVerbs;
		static UnorderedMap<String, Verbosity> _moduleVerbOverrides;
		static UnorderedMap<String, SharedPtr<Log>> _logInstances;
		static std::queue<Entry> _queue;
		static std::mutex _mutex;
		static std::condition_variable _cv;
		static std::atomic<bool> _exit;

	  public:
		static Log instance;

	  private:
		static void Process();

	  public:
		static SharedPtr<Log> Get(StringView module);
		static void CleanupExpired();
		static Verbosity GetVerbosity(const String &module);
		static Optional<Verbosity> GetVerbosityOverride(const String &module);
		static void SetVerbosityOverride(const String &module, Verbosity verb);
		static void UpdateVerbosities(const nlohmann::json &config);

	  private:
		String _module;

		bool CanOutput(StringView verb) const;
		void Output(StringView verb, const StringView &str) const;

	  public:
		explicit Log(const String &module);
		~Log();

		const String &GetModule() const;

		FORCEINLINE Verbosity GetVerbosity() const
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
			Output("Trace", Format(fmt, Forward<Args>(args)...));
		}
		template <typename... Args>
		FORCEINLINE void Debug(std::format_string<Args...> fmt, Args &&...args)
		{
			Output("Debug", Format(fmt, Forward<Args>(args)...));
		}
		template <typename... Args>
		FORCEINLINE void Info(std::format_string<Args...> fmt, Args &&...args)
		{
			Output("Info", Format(fmt, Forward<Args>(args)...));
		}
		template <typename... Args>
		FORCEINLINE void Warn(std::format_string<Args...> fmt, Args &&...args)
		{
			Output("Warn", Format(fmt, Forward<Args>(args)...));
		}
		template <typename... Args>
		FORCEINLINE void Error(std::format_string<Args...> fmt, Args &&...args)
		{
			Output("Error", Format(fmt, Forward<Args>(args)...));
		}
		template <typename... Args>
		FORCEINLINE void Fatal(std::format_string<Args...> fmt, Args &&...args)
		{
			Output("Fatal", Format(fmt, Forward<Args>(args)...));
		}
	};
} // namespace tudov

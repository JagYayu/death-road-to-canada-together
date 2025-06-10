#pragma once

#include "Defs.h"
#include "json.hpp"
#include <sol/lua_value.hpp>

#include <atomic>
#include <condition_variable>
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
		static SharedPtr<Log> Get(const String &module);
		static Verbosity GetVerbosity(const String &module);
		static Optional<Verbosity> GetVerbosityOverride(const String &module);
		static void SetVerbosityOverride(const String &module, Verbosity verb);
		static void UpdateVerbosities(const nlohmann::json &config);

	  private:
		String _module;

		bool CanOutput(const StringView &verb) const;
		void Output(const StringView &verb, const String &str) const;

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

		FORCEINLINE void Trace(const String &str) const
		{
			Output("Trace", str);
		}
		FORCEINLINE void Debug(const String &str) const
		{
			Output("Debug", str);
		}
		FORCEINLINE void Info(const String &str) const
		{
			Output("Info", str);
		}
		FORCEINLINE void Warn(const String &str) const
		{
			Output("Warn", str);
		}
		FORCEINLINE void Error(const String &str) const
		{
			Output("Error", str);
		}
		FORCEINLINE void Fatal(const String &str) const
		{
			Output("Fatal", str);
		}
	};
} // namespace tudov

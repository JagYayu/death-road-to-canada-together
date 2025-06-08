#pragma once

#include "String.h"
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
			std::string_view verbosity;
			std::string module;
			std::string message;
		};

	  public:
		enum class Verbosity
		{
			Trace,
			Debug,
			Info,
			Warn,
			Error,
		};

	  private:
		static std::queue<Entry> _queue;
		static std::mutex _mutex;
		static std::condition_variable _cv;
		static std::atomic<bool> _exit;

		std::string _module;

	  public:
		static Log instance;
		static std::unordered_map<std::string, Verbosity> moduleVerbosityOverrides;
		static Verbosity GetGlobalFlags(std::string module);

	  private:
		static void Process();

		void Output(const std::string_view verb, const std::string &str);

	  public:
		explicit Log(const std::string &module);
		~Log();

		bool CanDebug();

		inline void Trace(const std::string &str)
		{
			Output("Info", str);
		}
		inline void Debug(const std::string &str)
		{
			Output("Debug", str);
		}
		inline void Info(const std::string &str)
		{
			Output("Info", str);
		}
		inline void Warn(const std::string &str)
		{
			Output("Warn", str);
		}
		inline void Error(const std::string &str)
		{
			Output("Error", str);
		}
	};
} // namespace tudov

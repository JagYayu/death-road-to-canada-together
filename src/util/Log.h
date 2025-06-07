#pragma once

#include <sol/lua_value.hpp>

#include <atomic>
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
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

		static Log instance;
		static std::unordered_map<std::string, Verbosity> moduleVerbosityOverrides;
		static Verbosity GetGlobalFlags(std::string module);

	private:
		std::string _module;
		std::queue<Entry> queue;
		std::mutex mutex;
		std::condition_variable cv;
		std::thread worker;
		std::atomic<bool> exit;

		void Process();

	public:
		explicit Log(const std::string& module);
		~Log();

		bool CanDebug();

		void Debug(const std::string& str);
	};
}

#include "Log.h"

#include <chrono>
#include <format>

using namespace tudov;

Log Log::instance{"Log"};

std::unordered_map<std::string, Log::Verbosity> Log::moduleVerbosityOverrides{};

Log::Verbosity Log::GetGlobalFlags(std::string module)
{
	{
		auto &&it = moduleVerbosityOverrides.find("key");
		if (it != moduleVerbosityOverrides.end())
		{
			return it->second;
		}
	}
	return (Log::Verbosity)0;
}

void Log::Process()
{
	auto &&pred = [this]
	{
		return !queue.empty() || exit;
	};

	while (!exit)
	{
		std::unique_lock<std::mutex> lock{mutex};
		cv.wait(lock, pred);

		while (!queue.empty())
		{
			auto entry = std::move(queue.front());
			queue.pop();
			lock.unlock();

			std::cout << std::format("[{:%Y-%m-%d %H:%M:%S}] [{}] [{}] {}", entry.time, _module, entry.verbosity, entry.message) << std::endl;

			lock.lock();
		}
	}
}

Log::Log(const std::string &module)
	: _module(module),
	  exit(false)
{
	worker = std::thread(&Log::Process, this);
}

tudov::Log::~Log()
{
	{
		std::lock_guard<std::mutex> lock{mutex};
		exit = true;
	}
	cv.notify_all();
	worker.join();
}

bool Log::CanDebug()
{
	return false;
}

void Log::Debug(const std::string &str)
{
	{
		std::lock_guard<std::mutex> lock(mutex);
		auto &&entry = Entry();
		entry.time = std::chrono::system_clock::now();
		entry.verbosity = "Debug";
		entry.message = str;
		queue.push(entry);
	}
	cv.notify_one();
}

#include "Log.h"

#include <chrono>
#include <cstdint>
#include <format>

using namespace tudov;

static int32_t logCount = 0;
static std::thread logWorker;

std::queue<Log::Entry> Log::_queue;
std::mutex Log::_mutex;
std::condition_variable Log::_cv;
std::atomic<bool> Log::_exit = false;

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
	auto &&pred = []
	{
		return !_queue.empty() || _exit;
	};

	while (!_exit)
	{
		std::unique_lock<std::mutex> lock{_mutex};
		_cv.wait(lock, pred);

		while (!_queue.empty())
		{
			auto entry = std::move(_queue.front());
			_queue.pop();
			lock.unlock();

			std::cout << std::format("[{:%Y-%m-%d %H:%M:%S}] [{}] [{}] {}", entry.time, std::string_view(entry.module), entry.verbosity, std::string_view(entry.message)) << std::endl;

			lock.lock();
		}
	}
}

void Log::Output(const std::string_view verb, const std::string &str)
{
	{
		std::lock_guard<std::mutex> lock(_mutex);
		auto &&entry = Entry();
		entry.time = std::chrono::system_clock::now();
		entry.verbosity = verb;
		entry.module = _module;
		entry.message = str;
		_queue.push(entry);
	}
	_cv.notify_one();
}

Log::Log(const std::string &module)
    : _module(module)
{
	if (logCount == 0)
	{
		logWorker = std::thread(Log::Process);
	}
	logCount++;
}

tudov::Log::~Log()
{
	logCount--;
	if (logCount == 0)
	{
		{
			std::lock_guard<std::mutex> lock{_mutex};
			_exit = true;
		}
		_cv.notify_all();
		logWorker.join();
	}
}

bool Log::CanDebug()
{
	return false;
}

#include "Log.h"

#include <chrono>
#include <format>

using namespace tudov;

static Int32 logCount = 0;
static std::thread logWorker;

Log::Verbosity _globalVerbosity = tudov::Log::Verbosity::All;
UnorderedMap<String, Log::Verbosity> Log::_moduleVerbs{};
UnorderedMap<String, Log::Verbosity> Log::_moduleVerbOverrides{};
UnorderedMap<String, SharedPtr<Log>> Log::_logInstances{};
std::queue<Log::Entry> Log::_queue;
std::mutex Log::_mutex;
std::condition_variable Log::_cv;
std::atomic<bool> Log::_exit = false;
Log Log::instance{"Log"};

SharedPtr<Log> Log::Get(const String &module)
{
	auto &&it = _logInstances.find(module);
	if (it != _logInstances.end())
	{
		return it->second;
	}

	auto &&log = MakeShared<Log>(module);
	_logInstances.emplace(module, log);
	return log;
}

Log::Verbosity Log::GetVerbosity(const String &module)
{
	{
		auto &&it = _moduleVerbOverrides.find("key");
		if (it != _moduleVerbOverrides.end())
		{
			return it->second;
		}
	}
	return (Log::Verbosity)0;
}

Optional<Log::Verbosity> Log::GetVerbosityOverride(const String &module)
{
	auto &&it = _moduleVerbOverrides.find("key");
	if (it != _moduleVerbOverrides.end())
	{
		return it->second;
	}
	return null;
}

void Log::SetVerbosityOverride(const String &module, Verbosity verb)
{
	_moduleVerbOverrides[module] = verb;
}

void Log::UpdateVerbosities(const nlohmann::json &config)
{
	if (!config.is_object())
	{
		instance.Warn("Cannot update verbosities by receiving a variable that not a json object");
		return;
	}
}

Log::Log(const String &module)
    : _module(module)
{
	if (logCount == 0)
	{
		logWorker = std::thread(Log::Process);
	}
	logCount++;
}

Log::~Log()
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

			std::cout << Format("[{:%Y-%m-%d %H:%M:%S}] [{}] [{}] {}", entry.time, entry.module, entry.verbosity, entry.message) << std::endl;

			lock.lock();
		}
	}
}

bool Log::CanOutput(StringView verb) const
{

	return false;
}

void Log::Output(StringView verb, const String &str) const
{
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_queue.push(Entry{
		    .time = std::chrono::system_clock::now(),
		    .verbosity = verb,
		    .module = _module,
		    .message = str,
		});
	}
	_cv.notify_one();
}

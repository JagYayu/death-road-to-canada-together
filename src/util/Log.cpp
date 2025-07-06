#include "Log.h"
#include "Utils.hpp"

#include <chrono>
#include <format>
#include <fstream>

using namespace tudov;

static uint32_t logCount = 0;
static std::thread logWorker;

Log::EVerbosity _globalVerbosity = tudov::Log::EVerbosity::All;
std::unordered_map<std::string, Log::EVerbosity> Log::_moduleVerbs{};
std::unordered_map<std::string, Log::EVerbosity> Log::_moduleVerbOverrides{};
std::unordered_map<std::string, std::shared_ptr<Log>> Log::_logInstances{};
std::queue<Log::Entry> Log::_queue;
std::mutex Log::_mutex;
std::condition_variable Log::_cv;
std::atomic<bool> Log::_exit = false;
Log Log::instance{"Log"};

std::shared_ptr<Log> Log::Get(std::string_view module)
{
	auto &&str = std::string(module);
	auto &&it = _logInstances.find(str);
	if (it != _logInstances.end())
	{
		return it->second;
	}

	auto &&log = std::make_shared<Log>(str);
	_logInstances.try_emplace(str, log);
	return log;
}

void Log::CleanupExpired()
{
	for (auto &&it = _logInstances.begin(); it != _logInstances.end();)
	{
		if (it->first.empty() || it->second.use_count() <= 1)
		{
			it = _logInstances.erase(it);
		}
		else
		{
			++it;
		}
	}
	ShrinkUnorderedMap(_logInstances);
}

Log::EVerbosity Log::GetVerbosity(const std::string &module)
{
	{
		auto &&it = _moduleVerbOverrides.find("key");
		if (it != _moduleVerbOverrides.end())
		{
			return it->second;
		}
	}
	return (Log::EVerbosity)0;
}

std::optional<Log::EVerbosity> Log::GetVerbosityOverride(const std::string &module)
{
	auto &&it = _moduleVerbOverrides.find("key");
	if (it != _moduleVerbOverrides.end())
	{
		return it->second;
	}
	return std::nullopt;
}

void Log::SetVerbosityOverride(const std::string &module, EVerbosity verb)
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

void Log::Exit() noexcept
{
	{
		std::lock_guard<std::mutex> lock{_mutex};
		_exit = true;
	}
	_cv.notify_all();

	if (logWorker.joinable())
	{
		logWorker.join();
	}
}

Log::Log(const std::string &module) noexcept
    : _module(module)
{
	if (logCount == 0)
	{
		logWorker = std::thread(Log::Process);
	}
	logCount++;
}

Log::~Log() noexcept
{
	logCount--;
	if (logCount == 0)
	{
		Exit();
	}
}

void Log::Process()
{
	auto &&pred = []
	{
		return !_queue.empty() || _exit;
	};

	std::filesystem::create_directories("logs");
	std::ofstream logFile{
	    std::format("logs/{:%Y_%m_%d_%H_%M}.log", std::chrono::system_clock::now()),
	    std::ios::app};

	while (!_exit)
	{
		std::unique_lock<std::mutex> lock{_mutex};
		_cv.wait(lock, pred);

		while (!_queue.empty())
		{
			auto entry = std::move(_queue.front());
			_queue.pop();
			lock.unlock();

			auto &&msg = std::format("[{:%Y-%m-%d %H:%M:%S}] [{}] [{}] {}", entry.time, entry.module, entry.verbosity, entry.message);
			std::cout << msg << std::endl;
			logFile << msg << '\n';
			logFile.flush();

			lock.lock();
		}
	}
}

bool Log::CanOutput(std::string_view verb) const
{
	return true;
}

void Log::Output(std::string_view verb, const std::string_view &str) const
{
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_queue.push(Entry{
		    .time = std::chrono::system_clock::now(),
		    .verbosity = verb,
		    .module = _module,
		    .message = std::string(str),
		});
	}
	_cv.notify_one();
}

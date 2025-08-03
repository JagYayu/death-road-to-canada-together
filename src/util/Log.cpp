#include "util/Log.hpp"
#include "util/Utils.hpp"

#include "json.hpp"

#include <cassert>
#include <chrono>
#include <format>
#include <fstream>
#include <memory>
#include <type_traits>

using namespace tudov;

static constexpr decltype(auto) defaultModule = "Log";
static uint32_t logCount = 0;
static std::unique_ptr<std::thread> logWorker;

Log::EVerbosity Log::_globalVerbosities = tudov::Log::EVerbosity::All;
std::unordered_map<std::string, Log::EVerbosity> Log::_verbosities{};
std::unordered_map<std::string, Log::EVerbosity> Log::_verbositiesOverrides{};
std::unordered_map<std::string, std::shared_ptr<Log>> Log::_logInstances{};
std::queue<Log::Entry> Log::_queue;
std::mutex Log::_mutex;
std::condition_variable Log::_cv;
std::atomic<bool> Log::_exit = false;

std::shared_ptr<Log> Log::Get(std::string_view module) noexcept
{
	// * If Error occurred here, are you attempt to call `Log::Get` in static initialization?

	auto &&str = std::string(module);
	auto &&it = _logInstances.find(str);
	if (it != _logInstances.end()) [[likely]]
	{
		return it->second;
	}

	auto &&log = std::make_shared<Log>(str);
	_logInstances.try_emplace(str, log);
	return log;
}

Log &Log::GetInstance() noexcept
{
	return *Get(defaultModule);
}

void Log::CleanupExpired() noexcept
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

void Log::Quit() noexcept
{
	OutputImpl(defaultModule, VerbDebug, "Logging system quitting");

	CleanupExpired();

	if (!_logInstances.empty())
	{
		OutputImpl(defaultModule, VerbWarn, "Unreleased log pointers detected on application quit. Maybe memory leaked?");
		for (auto &&[name, log] : _logInstances)
		{
			OutputImpl(defaultModule, VerbWarn, std::format("Log name \"{}\", ref count {}", name.data(), log.use_count()));
		}
	}

	Exit();
}

std::optional<Log::EVerbosity> Log::GetVerbosity(const std::string &module) noexcept
{
	if (auto &&it = _verbositiesOverrides.find(module); it != _verbositiesOverrides.end())
	{
		return it->second;
	}
	return std::nullopt;
}

bool Log::SetVerbosity(const std::string &module, EVerbosity verb) noexcept
{
	if (auto &&it = _verbositiesOverrides.find(module); it != _verbositiesOverrides.end())
	{
		it->second = verb;
		return true;
	}
	return false;
}

void Log::UpdateVerbosities(const void *jsonConfig)
{
	auto config = *static_cast<const nlohmann::json *>(jsonConfig);
	if (!config.is_object())
	{
		GetInstance().Warn("Cannot update verbosities by receiving a variable that not a json object");
		return;
	}

	if (auto &&global = config["global"]; global.is_number_integer())
	{
		_globalVerbosities = EVerbosity(global.get<std::underlying_type<EVerbosity>::type>());
	}

	if (auto &&module = config["module"]; module.is_object())
	{
		for (auto &&[key, value] : module.items())
		{
			if (value.is_number_integer())
			{
				_verbosities[key] = EVerbosity(value.get<std::underlying_type<EVerbosity>::type>());
			}
		}
	}
}

std::optional<Log::EVerbosity> Log::GetVerbosityOverride(const std::string &module) noexcept
{
	auto &&it = _verbositiesOverrides.find("key");
	if (it != _verbositiesOverrides.end())
	{
		return it->second;
	}
	return std::nullopt;
}

void Log::SetVerbosityOverride(const std::string &module, EVerbosity verb) noexcept
{
	_verbositiesOverrides[module] = verb;
}

void Log::Exit() noexcept
{
	if (logWorker == nullptr)
	{
		return;
	}

	{
		std::lock_guard<std::mutex> lock{_mutex};
		_exit = true;
	}
	_cv.notify_all();

	if (logWorker->joinable())
	{
		logWorker->join();
		logWorker = nullptr;
	}
}

std::size_t Log::CountLogs() noexcept
{
	return _logInstances.size();
}

std::unordered_map<std::string, std::shared_ptr<Log>>::const_iterator Log::BeginLogs() noexcept
{
	return _logInstances.cbegin();
}

std::unordered_map<std::string, std::shared_ptr<Log>>::const_iterator Log::EndLogs() noexcept
{
	return _logInstances.cend();
}

std::size_t Log::CountVerbosities() noexcept
{
	return _verbosities.size();
}

std::unordered_map<std::string, Log::EVerbosity>::const_iterator Log::BeginVerbosities() noexcept
{
	return _verbosities.cbegin();
}

std::unordered_map<std::string, Log::EVerbosity>::const_iterator Log::EndVerbosities() noexcept
{
	return _verbosities.cend();
}

std::size_t Log::CountVerbositiesOverrides() noexcept
{
	return _verbositiesOverrides.size();
}

std::unordered_map<std::string, Log::EVerbosity>::const_iterator Log::BeginVerbositiesOverrides() noexcept
{
	return _verbositiesOverrides.cbegin();
}

std::unordered_map<std::string, Log::EVerbosity>::const_iterator Log::EndVerbositiesOverrides() noexcept
{
	return _verbositiesOverrides.cbegin();
}

Log::Log(const std::string &module) noexcept
    : _module(module)
{
	if (logCount == 0)
	{
		assert(logWorker == nullptr);
		logWorker = std::make_unique<std::thread>(Log::Process);

		OutputImpl(defaultModule, VerbDebug, "Logging system initialized");
	}
	++logCount;
}

Log::~Log() noexcept
{
	--logCount;
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

bool Log::CanOutput(Log::EVerbosity verb) const noexcept
{
	return true;
}

void Log::Output(std::string_view verb, std::string_view str) const noexcept
{
	OutputImpl(_module, verb, str);
}

void Log::OutputImpl(std::string_view module, std::string_view verb, std::string_view str)
{
	{
		std::lock_guard<std::mutex> lock{_mutex};
		_queue.push(Entry{
		    .time = std::chrono::system_clock::now(),
		    .verbosity = verb,
		    .module = std::string(module),
		    .message = std::string(str),
		});
	}
	_cv.notify_one();
}

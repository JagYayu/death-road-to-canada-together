/**
 * @file system/Log.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "System/Log.hpp"
#include "System/LogVerbosity.hpp"
#include "Util/EnumFlag.hpp"
#include "Util/StringUtils.hpp"
#include "Util/Utils.hpp"

#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <thread>
#include <tuple>
#include <type_traits>

using namespace tudov;

constexpr bool SingleThread = false;

static constexpr decltype(auto) defaultModule = TE_NAMEOF(Log);
static uint32_t logCount = 0;
static std::unique_ptr<std::thread> logWorker = nullptr;
static std::ofstream fileStream;

ELogVerbosity Log::_globalVerbosities = ELogVerbosity::All;
std::unordered_map<std::string, ELogVerbosity> Log::_verbosities{};
std::unordered_map<std::string, ELogVerbosity> Log::_verbositiesOverrides{};
std::unordered_map<std::string, std::shared_ptr<Log>> Log::_logInstances{};
std::queue<Log::Entry> Log::_queue;
std::mutex Log::_mutex;
std::condition_variable Log::_cv;
std::atomic<bool> Log::_exit = false;

std::shared_ptr<Log> Log::Get(std::string_view module) noexcept
{
	// ! If an error was occurred here, It must be an attempt to call after the logging system is quitted.
	// ! Are you attempt to call `Log::Get` in static initialization?

	auto &&str = std::string(module);
	auto it = _logInstances.find(str);
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
	for (auto it = _logInstances.begin(); it != _logInstances.end();)
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
	OutputImpl(defaultModule, VerbDebug, "Logging system is quitting");

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

ELogVerbosity Log::GetVerbosities(std::string_view module) noexcept
{
	{
		auto verbs = GetVerbositiesOverride(module.data());
		if (verbs.has_value())
		{
			return verbs.value();
		}
	}

	{
		if (auto it = _verbositiesOverrides.find(module.data()); it != _verbositiesOverrides.end())
		{
			return it->second;
		}
	}

	return _globalVerbosities;
}

bool Log::SetVerbosities(std::string_view module, ELogVerbosity flags) noexcept
{
	if (auto it = _verbositiesOverrides.find(module.data()); it != _verbositiesOverrides.end())
	{
		it->second = flags;
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
		_globalVerbosities = ELogVerbosity(global.get<std::underlying_type<ELogVerbosity>::type>());
	}

	if (auto &&module = config["module"]; module.is_object())
	{
		for (auto &&[key, value] : module.items())
		{
			if (value.is_number_integer())
			{
				_verbosities[key] = ELogVerbosity(value.get<std::underlying_type<ELogVerbosity>::type>());
			}
		}
	}
}

std::optional<ELogVerbosity> Log::GetVerbositiesOverride(const std::string &module) noexcept
{
	auto it = _verbositiesOverrides.find("key");
	if (it != _verbositiesOverrides.end())
	{
		return it->second;
	}
	return std::nullopt;
}

void Log::SetVerbositiesOverride(std::string_view module, ELogVerbosity flags) noexcept
{
	_verbositiesOverrides[module.data()] = flags;
}

bool Log::RemoveVerbositiesOverride(std::string_view module) noexcept
{
	return _verbositiesOverrides.erase(module.data());
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

std::unordered_map<std::string, ELogVerbosity>::const_iterator Log::BeginVerbosities() noexcept
{
	return _verbosities.cbegin();
}

std::unordered_map<std::string, ELogVerbosity>::const_iterator Log::EndVerbosities() noexcept
{
	return _verbosities.cend();
}

std::size_t Log::CountVerbositiesOverrides() noexcept
{
	return _verbositiesOverrides.size();
}

std::unordered_map<std::string, ELogVerbosity>::const_iterator Log::BeginVerbositiesOverrides() noexcept
{
	return _verbositiesOverrides.cbegin();
}

std::unordered_map<std::string, ELogVerbosity>::const_iterator Log::EndVerbositiesOverrides() noexcept
{
	return _verbositiesOverrides.cbegin();
}

Log::Log(std::string_view module) noexcept
    : _module(std::string(module))
{
	if (logCount == 0 && logWorker == nullptr)
	{
		std::filesystem::create_directories("Logs");

		logWorker = std::make_unique<std::thread>(Log::Process);

		OutputImpl(defaultModule, VerbDebug, "Logging system initialized");
	}
	++logCount;
}

Log::~Log() noexcept
{
	--logCount;
	TE_ASSERT(logCount >= 0);
}

void Log::Process() noexcept
{
	if (SingleThread)
	{
		return;
	}

	auto &&pred = []
	{
		return !_queue.empty() || _exit;
	};

	fileStream = std::ofstream{
	    std::format("logs/{:%Y_%m_%d_%H_%M}.log", std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now())),
	    std::ios::app,
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

			std::string message = std::format("[{:%Y-%m-%d %H:%M:%S}] [{}] [{}] {}",
			                                  std::chrono::zoned_time(std::chrono::current_zone(), entry.time),
			                                  entry.module,
			                                  entry.verbosity,
			                                  entry.message);

			PrintToConsole(message, entry);

			fileStream << message << '\n';
			lock.lock();
		}

		fileStream.flush();
		std::cout.flush();

		_queue = std::queue<Log::Entry>();
	}
}

void Log::PrintToConsole(std::string_view message, const Entry &entry) noexcept
{
	std::string_view color;
	if (entry.verbosity == VerbTrace)
		color = "\033[38;5;246m";
	else if (entry.verbosity == VerbDebug)
		color = "\033[38;5;39m";
	else if (entry.verbosity == VerbInfo)
		color = "\033[38;5;34m";
	else if (entry.verbosity == VerbWarn)
		color = "\033[38;5;220m";
	else if (entry.verbosity == VerbError)
		color = "\033[38;5;196m";
	else if (entry.verbosity == VerbFatal)
		color = "\033[37;41;1m";
	else
		color = "";

	if (color != "")
	{
		std::cout << color << message << "\033[0m\n";
	}
	else
	{
		std::cout << message << '\n';
	}
}

bool Log::CanOutput(ELogVerbosity flag) const noexcept
{
	return EnumFlag::HasAll(GetVerbosities(), flag);
}

void Log::Output(std::string_view verb, std::string_view str) const noexcept
{
	OutputImpl(_module, verb, str);
}

void Log::OutputImpl(std::string_view module, std::string_view verb, std::string_view message)
{
	Entry entry{
	    .time = std::chrono::system_clock::now(),
	    .verbosity = verb,
	    .module = std::string(module),
	    .message = std::string(message),
	};

	if (SingleThread)
	{
		std::string message = std::format("[{:%Y-%m-%d %H:%M:%S}] [{}] [{}] {}",
		                                  std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now()),
		                                  entry.module,
		                                  entry.verbosity,
		                                  entry.message);

		PrintToConsole(message, entry);
		fileStream << message << std::endl;
	}
	else
	{
		if (!_exit) [[likely]]
		{
			std::lock_guard<std::mutex> lock{_mutex};

			_queue.push(entry);
		}

		_cv.notify_one();
	}
}

void Log::LuaOutput(sol::object verb, sol::table args) const noexcept
{
	static std::string buffer;
	const auto bufferMaxSize = 1024;

	for (std::uint8_t i = 1; i <= 9; ++i)
	{
		auto value = args[i];
		if (value.valid())
		{
			auto &&str = StringUtils::Unescape(value.get<std::string_view>());
			buffer.append(str);
		}
		else
		{
			break;
		}
	}

	Output(verb.as<std::string_view>(), buffer.c_str());

	buffer.clear();
}

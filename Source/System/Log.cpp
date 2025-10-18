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
#include <vector>

using namespace tudov;

constexpr bool SingleThread = false;

static constexpr decltype(auto) defaultModule = TE_NAMEOF(Log);
static uint32_t logCount = 0;
static std::unique_ptr<std::thread> logWorker = nullptr;
static std::ofstream fileStream;

ELogVerbosity Log::_globalVerbosities = ELogVerbosity::All;
std::unordered_map<std::string, ELogVerbosity> Log::_verbosities{};
std::unordered_map<std::string, std::shared_ptr<Log>> Log::_logInstanceMap{};
std::unique_ptr<std::vector<Log *>> Log::_logInstanceList{};
std::queue<Log::Entry> Log::_queue;
std::mutex Log::_mutex;
std::condition_variable Log::_cv;
std::atomic<bool> Log::_exit = false;

std::shared_ptr<Log> Log::Get(std::string_view module) noexcept
{
	// ! If an error was occurred here, It must be an attempt to call after the logging system is quitted.
	// ! Are you attempt to call `Log::Get` in static initialization?

	auto &&str = std::string(module);
	auto it = _logInstanceMap.find(str);
	if (it != _logInstanceMap.end()) [[likely]]
	{
		return it->second;
	}

	auto &&log = std::make_shared<Log>(str);
	_logInstanceMap.try_emplace(str, log);
	_verbosities.emplace(str, _globalVerbosities);
	_logInstanceList = nullptr;
	return log;
}

Log &Log::GetInstance() noexcept
{
	return *Get(defaultModule);
}

void Log::CleanupExpired() noexcept
{
	for (auto it = _logInstanceMap.begin(); it != _logInstanceMap.end();)
	{
		if (it->first.empty() || it->second.use_count() <= 1)
		{
			it = _logInstanceMap.erase(it);
			_logInstanceList = nullptr;
		}
		else
		{
			++it;
		}
	}
	ShrinkUnorderedMap(_logInstanceMap);
}

void Log::Quit() noexcept
{
	OutputImpl(defaultModule, VerbDebug, "Logging system is quitting");

	CleanupExpired();

	if (!_logInstanceMap.empty())
	{
		OutputImpl(defaultModule, VerbWarn, "Unreleased log pointers detected on application quit. Maybe memory leaked?");
		for (auto &&[name, log] : _logInstanceMap)
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

	_logInstanceMap = {};
	_verbosities = {};
	_logInstanceList = nullptr;
}

std::string_view Log::GetModule() const noexcept
{
	return _module;
}

ELogVerbosity Log::GetVerbosities(std::string_view module) noexcept
{
	if (auto it = _verbosities.find(module.data()); it != _verbosities.end())
	{
		return it->second;
	}
	return _globalVerbosities;
}

bool Log::SetVerbosities(std::string_view module, ELogVerbosity flags) noexcept
{
	if (auto it = _verbosities.find(module.data()); it != _verbosities.end())
	{
		it->second = flags;
		return true;
	}

	return false;
}

void Log::SaveVerbosities(void *jsonConfig)
{
	auto &&config = *static_cast<nlohmann::json *>(jsonConfig);
	if (!config.is_object()) [[unlikely]]
	{
		GetInstance().Warn("Cannot update verbosities by receiving a variable that not a json object");
		return;
	}

	config["global"] = _globalVerbosities;

	config["module"] = {};
	auto &&modules = config["module"];
	for (auto &&[module, verb] : _verbosities)
	{
		if (verb != _globalVerbosities)
		{
			modules[module] = verb;
		}
	}
}

void Log::LoadVerbosities(const void *jsonConfig)
{
	auto &&config = *static_cast<const nlohmann::json *>(jsonConfig);
	if (!config.is_object()) [[unlikely]]
	{
		GetInstance().Warn("Cannot load verbosities by receiving a variable that not a json object");
		return;
	}

	if (auto &&global = config["global"]; global.is_number_integer())
	{
		_globalVerbosities = ELogVerbosity(global.get<std::underlying_type<ELogVerbosity>::type>());
	}
	else
	{
		_globalVerbosities = EnumFlag::BitOr(ELogVerbosity::Fatal, ELogVerbosity::Error, ELogVerbosity::Warn, ELogVerbosity::Info);
	}

	if (auto &&module = config["module"]; module.is_object())
	{
		for (auto &&[key, value] : module.items())
		{
			if (value.is_number_integer())
			{
				_verbosities[key] = static_cast<ELogVerbosity>(value.get<std::underlying_type<ELogVerbosity>::type>());
			}
			else
			{
				_verbosities[key] = _globalVerbosities;
			}
		}
	}
}

std::size_t Log::CountLogs() noexcept
{
	return _logInstanceMap.size();
}

std::vector<Log *>::const_iterator Log::BeginLogs() noexcept
{
	return GetOrNewLogInstanceList().cbegin();
}

std::vector<Log *>::const_iterator Log::EndLogs() noexcept
{
	return GetOrNewLogInstanceList().cend();
}

std::vector<Log *> &Log::GetOrNewLogInstanceList() noexcept
{
	if (_logInstanceList == nullptr) [[unlikely]]
	{
		_logInstanceList = std::make_unique<std::vector<Log *>>();

		for (auto [_, logInstance] : _logInstanceMap)
		{
			_logInstanceList->emplace_back(logInstance.get());
		}

		std::sort(_logInstanceList->begin(), _logInstanceList->end(), [](Log *l, Log *r) -> bool
		{
			TE_ASSERT(l->_module != r->_module);
			return l->_module < r->_module;
		});
	}

	return *_logInstanceList;
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

bool Log::LuaCanOutput(sol::object verb) const noexcept
{
	if (verb.is<std::double_t>())
	{
		return CanOutput(verb.as<ELogVerbosity>());
	}
	else
	{
		return false;
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

	if (verb.is<std::double_t>())
	{
		switch (verb.as<ELogVerbosity>())
		{
		case ELogVerbosity::Fatal:
			Fatal("{}", buffer.c_str());
			break;
		case ELogVerbosity::Error:
			Error("{}", buffer.c_str());
			break;
		case ELogVerbosity::Warn:
			Warn("{}", buffer.c_str());
			break;
		case ELogVerbosity::Info:
			Info("{}", buffer.c_str());
			break;
		case ELogVerbosity::Debug:
			Debug("{}", buffer.c_str());
			break;
		case ELogVerbosity::Trace:
			Trace("{}", buffer.c_str());
			break;
		default:
			break;
		}
	}
	else if (verb.is<std::string_view>())
	{
		Output(verb.as<std::string_view>(), buffer.c_str());
	}

	buffer.clear();
}

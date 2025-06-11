#include "EngineConfig.h"

#include "lib/json.hpp"
#include "util/Defs.h"
#include "util/Log.h"

#include <fstream>

using namespace tudov;

constexpr const char *keyFullscreen = "fullscreen";
constexpr const char *keyHeight = "height";
constexpr const char *keyLog = "log";
constexpr const char *keyTitle = "title";
constexpr const char *keyWidth = "width";
constexpr const char *keyWindow = "window";

nlohmann::json GetLog(nlohmann::json &config)
{
	auto &&log = config[keyLog];
	if (!log.is_object())
	{
		config[keyLog] = nlohmann::json::object();
	}
	return log;
}

nlohmann::json GetWindow(nlohmann::json &config)
{
	auto &&window = config[keyWindow];
	if (!window.is_object())
	{
		window = {
		    {keyWidth, 1280},
		    {keyHeight, 720},
		    {keyFullscreen, false},
		};
		config[keyWindow] = window;
	}
	return window;
}

EngineConfig::EngineConfig()
    : _log(Log::Get("EngineConfig"))
{
	EngineConfig::Load();

	_fileWatcher = MakeUnique<filewatch::FileWatch<String>>(String(file), [&](StringView path, const filewatch::Event changeType)
	{
		EngineConfig::Load();
	});
}

EngineConfig::~EngineConfig()
{
	Save();
}

void EngineConfig::Save()
{
	std::ofstream out{String(file)};
	if (out.is_open())
	{
		out << _config.dump(1, '\t');
		out.close();
	}
	else
	{
		_log->Error(Format("Failed to open config file for writing: ", String(file)));
	}
}

void EngineConfig::Load()
{
	std::ifstream f{String(file)};
	if (f.is_open())
	{
		_config = nlohmann::json::parse(f);
		f.close();
	}
	else
	{
		_config = nlohmann::json();
		std::ofstream out{String(file)};
		out << _config.dump(4);
		out.close();
	}

	Log::UpdateVerbosities(GetLog(_config));
}

StringView EngineConfig::GetWindowTitle()
{
	auto &&title = GetWindow(_config)[keyTitle];
	return title.is_string() ? StringView(title) : defaultWindowTitle;
}

UInt32 EngineConfig::GetWindowWidth()
{
	auto &&width = GetWindow(_config)[keyWidth];
	return width.is_number() ? width.get<UInt32>() : defaultWindowWidth;
}

UInt32 EngineConfig::GetWindowHeight()
{
	auto &&width = GetWindow(_config)[keyHeight];
	return width.is_number() ? width.get<UInt32>() : defaultWindowHeight;
}

void EngineConfig::SetWindowTitle(const String &)
{
}

void EngineConfig::SetWindowWidth(UInt32)
{
}

void EngineConfig::SetWindowHeight(UInt32)
{
}

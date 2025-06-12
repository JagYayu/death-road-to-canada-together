#include "EngineConfig.h"

#include "lib/json.hpp"
#include "util/Defs.h"
#include "util/Log.h"

#include <fstream>

using namespace tudov;

constexpr const char *keyDirectories = "directories";
constexpr const char *keyFiles = "files";
constexpr const char *keyFullscreen = "fullscreen";
constexpr const char *keyHeight = "height";
constexpr const char *keyLog = "log";
constexpr const char *keyMount = "mount";
constexpr const char *keyTitle = "title";
constexpr const char *keyWidth = "width";
constexpr const char *keyWindow = "window";

static const auto valueWindowFullscreen = false;
static const auto valueWindowHeight = 720;
static const auto valueWindowTitle = "DR2C Together";
static const auto valueWindowWidth = 1280;
static const auto valueMountDirectories = Vector<String>{
    "data",
    "gfx",
};
static const auto valueMountFiles = UnorderedMap<String, ResourceType>{
    {".png", ResourceType::Texture},
    {".ogg", ResourceType::Audio},
};

nlohmann::json &GetLog(nlohmann::json &config)
{
	auto &&log = config[keyLog];
	if (!log.is_object())
	{
		config[keyLog] = nlohmann::json::object();
	}
	return log;
}

nlohmann::json &GetMount(nlohmann::json &config)
{
	auto &&mount = config[keyMount];
	if (!mount.is_object())
	{
		mount = {
		    {keyDirectories, valueMountDirectories},
		};
		config[keyMount] = mount;
	}
	return mount;
}

nlohmann::json &GetWindow(nlohmann::json &config)
{
	auto &&window = config[keyWindow];
	if (!window.is_object())
	{
		window = {
		    {keyWidth, valueWindowWidth},
		    {keyHeight, valueWindowHeight},
		    {keyFullscreen, valueWindowFullscreen},
		    {keyTitle, valueWindowTitle},
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

void EngineConfig::Save() noexcept
{
	try
	{
		String f{file};
		std::ofstream out{f};
		if (out.is_open())
		{
			out << _config.dump(1, '\t');
			out.close();
			_log->Trace("Config file saved: ", f);
		}
		else
		{
			_log->Error("Failed to open config file for writing: ", f);
		}
	}
	catch (const std::exception &e)
	{
		_log->Error("Exception occurred while saving config file: {}", e.what());
	}
}

void EngineConfig::Load() noexcept
{
	try
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
	catch (const std::exception &e)
	{
		_log->Error("Exception occurred while loading config file: {}", e.what());
	}
}

Vector<String> EngineConfig::GetMountDirectories()
{
	auto &&mount = GetMount(_config);
	auto &&directories = mount[keyDirectories];
	if (!directories.is_array())
	{
		directories = valueMountDirectories;
		mount[keyDirectories] = directories;
	}
	return directories;
}

UnorderedMap<String, ResourceType> EngineConfig::GetMountFiles()
{
	auto &&mount = GetMount(_config);
	auto &&files = mount[keyFiles];
	if (!files.is_object())
	{
		files = valueMountFiles;
		mount[keyFiles] = files;
	}
	return files;
}

StringView EngineConfig::GetWindowTitle()
{
	auto &&window = GetWindow(_config);
	auto &&title = window[keyTitle];
	if (!title.is_string())
	{
		title = defaultWindowTitle;
		window[keyTitle] = title;
	}
	return title;
}

UInt32 EngineConfig::GetWindowWidth()
{
	auto &&window = GetWindow(_config);
	auto &&width = window[keyWidth];
	if (!width.is_number_integer())
	{
		width = defaultWindowWidth;
		window[keyWidth] = width;
	}
	return width;
}

UInt32 EngineConfig::GetWindowHeight()
{
	auto &&window = GetWindow(_config);
	auto &&height = window[keyHeight];
	if (!height.is_number_integer())
	{
		height = defaultWindowHeight;
		window[keyHeight] = height;
	}
	return height;
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

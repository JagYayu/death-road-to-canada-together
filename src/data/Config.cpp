#include "Config.hpp"

#include "SDL3/SDL_properties.h"
#include "util/Defs.hpp"
#include "util/Log.hpp"

#include <json.hpp>

#include <fstream>

using namespace tudov;

constexpr const char *keyBitmaps = "bitmaps";
constexpr const char *keyDirectories = "directories";
constexpr const char *keyFiles = "files";
constexpr const char *keyFramelimit = "framelimit";
constexpr const char *keyFullscreen = "fullscreen";
constexpr const char *keyHeight = "height";
constexpr const char *keyLog = "log";
constexpr const char *keyMount = "mount";
constexpr const char *keyRenderBackend = "renderBackend";
constexpr const char *keyTitle = "title";
constexpr const char *keyWidth = "width";
constexpr const char *keyWindow = "window";

static const auto valueWindowFramelimit = 100;
static const auto valueWindowFullscreen = false;
static const auto valueWindowHeight = 720;
static const auto valueWindowTitle = "DR2C Together";
static const auto valueWindowWidth = 1280;
static const auto valueMountBitmaps = std::vector<std::string>{
    R"(fonts/.*\.png)",
};
static const auto valueMountDirectories = std::vector<std::string>{
    "assets",
};
static const auto valueMountFiles = std::unordered_map<std::string, ResourceType>{
    {".png", ResourceType::Image},
    {".ogg", ResourceType::Audio},
};

nlohmann::json &GetLog(nlohmann::json &config) noexcept
{
	auto &&log = config[keyLog];
	if (!log.is_object())
	{
		config[keyLog] = nlohmann::json::object();
	}
	return log;
}

nlohmann::json &GetMount(nlohmann::json &config) noexcept
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

nlohmann::json &GetWindow(nlohmann::json &config) noexcept
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

Config::Config() noexcept
    : _log(Log::Get("EngineConfig"))
{
	Config::Load();

	_fileWatcher = new filewatch::FileWatch<std::string>(std::string(ConfigFile), [&](std::string_view path, const filewatch::Event changeType)
	{
		Config::Load();
	});
}

Config::~Config() noexcept
{
	Save();
	delete _fileWatcher;
}

void Config::Save() noexcept
{
	try
	{
		std::string f{ConfigFile};
		std::ofstream out{f};
		if (out.is_open())
		{
			out << _config.dump(1, '\t');
			out.close();
			_log->Trace("Config file saved: \"{}\"", f);
		}
		else
		{
			_log->Error("Failed to open config file for writing: \"{}\"", f);
		}
	}
	catch (const std::exception &e)
	{
		_log->Error("Exception occurred while saving config file: {}", e.what());
	}
}

void Config::Load() noexcept
{
	try
	{
		std::ifstream f{std::string(ConfigFile)};
		if (f.is_open())
		{
			_config = nlohmann::json::parse(f);
			f.close();
		}
		else
		{
			_config = nlohmann::json();
			std::ofstream out{std::string(ConfigFile)};
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

std::vector<std::string> Config::GetMountBitmaps() noexcept
{
	auto &&mount = GetMount(_config);
	auto &&bitmaps = mount[keyBitmaps];
	if (!bitmaps.is_array())
	{
		bitmaps = valueMountBitmaps;
		mount[keyBitmaps] = bitmaps;
	}
	return bitmaps;
}

std::vector<std::string> Config::GetMountDirectories() noexcept
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

std::unordered_map<std::string, ResourceType> Config::GetMountFiles() noexcept
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

bool Config::GetRenderBackend() noexcept
{
	// auto &&renderBackend = _config[keyRenderBackend];
	// if (!renderBackend.is_string())
	// {
	// 	renderBackend = valueRenderBackend;
	// 	_config[keyRenderBackend] = renderBackend;
	// }
	// return renderBackend;
	return false;
}

std::uint32_t Config::GetWindowFramelimit() noexcept
{
	auto &&window = GetWindow(_config);
	auto &&framelimit = window[keyFramelimit];
	if (!framelimit.is_number())
	{
		framelimit = valueWindowFramelimit;
		window[keyFramelimit] = framelimit;
	}
	return framelimit;
}

std::string_view Config::GetWindowTitle() noexcept
{
	auto &&window = GetWindow(_config);
	auto &&title = window[keyTitle];
	if (!title.is_string())
	{
		title = WindowTitle;
		window[keyTitle] = title;
	}
	return title;
}

std::uint32_t Config::GetWindowWidth() noexcept
{
	auto &&window = GetWindow(_config);
	auto &&width = window[keyWidth];
	if (!width.is_number_integer())
	{
		width = WindowWidth;
		window[keyWidth] = width;
	}
	return width;
}

std::uint32_t Config::GetWindowHeight() noexcept
{
	auto &&window = GetWindow(_config);
	auto &&height = window[keyHeight];
	if (!height.is_number_integer())
	{
		height = WindowHeight;
		window[keyHeight] = height;
	}
	return height;
}

void Config::SetWindowTitle(const std::string &) noexcept
{
}

void Config::SetWindowWidth(std::uint32_t) noexcept
{
}

void Config::SetWindowHeight(std::uint32_t) noexcept
{
}

std::uint32_t Config::GetPropertiesID() noexcept
{
	if (!_propertiesID.has_value()) [[unlikely]]
	{
		_propertiesID = SDL_CreateProperties();
	}
	return *_propertiesID;
}

std::int64_t Config::GetCustom(std::string_view key, std::int64_t default_) noexcept
{
	return SDL_GetNumberProperty(GetPropertiesID(), key.data(), default_);
}

void Config::SetCustom(std::string_view key, std::int64_t value) noexcept
{
	SDL_SetNumberProperty(GetPropertiesID(), key.data(), value);
}

/**
 * @file data/Config.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "data/Config.hpp"

#include "data/Constants.hpp"
#include "data/PathType.hpp"
#include "util/FileSystemWatch.hpp"
#include "system/Log.hpp"

#include "SDL3/SDL_properties.h"
#include "system/LogMicros.hpp"
#include <json.hpp>

#include <fstream>

using namespace tudov;

static constexpr const char *keyBitmaps = "bitmaps";
static constexpr const char *keyDirectories = "directories";
static constexpr const char *keyFiles = "files";
static constexpr const char *keyFramelimit = "framelimit";
static constexpr const char *keyFullscreen = "fullscreen";
static constexpr const char *keyHeight = "height";
static constexpr const char *keyLog = "log";
static constexpr const char *keyMount = "mount";
static constexpr const char *keyRenderBackend = "renderBackend";
static constexpr const char *keyTitle = "title";
static constexpr const char *keyWidth = "width";
static constexpr const char *keyWindow = "window";

static const auto valueLog = nlohmann::json::object({
    {"global", -1},
    {"module", nlohmann::json::object()},
});
static const auto valueMountBitmaps = std::vector<std::string>{
    R"(fonts/.*\.png)",
};
static const auto valueMountDirectories = std::vector<std::string>{
    "data",
    "gfx",
};
static const auto valueMountFiles = std::unordered_map<std::string, EResourceType>{
    {".png", EResourceType::Image},
    {".ogg", EResourceType::Audio},
};
static const auto valueWindowFramelimit = 60;
static const auto valueWindowFullscreen = false;
static const auto valueWindowHeight = 720;
static const auto valueWindowTitle = "DR2C Together";
static const auto valueWindowWidth = 1280;

Log &Config::GetLog() noexcept
{
	return *Log::Get("Config");
}

nlohmann::json &GetLog(nlohmann::json &config) noexcept
{
	auto &&log = config[keyLog];
	if (!log.is_object())
	{
		config[keyLog] = valueLog;
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
{
	Config::Load();

	_fileWatch = std::make_unique<FileSystemWatch>(std::string(AppConfigFile));
	_fileWatch->GetOnCallback() += [this](std::string_view path, EPathType pathType, EFileChangeType changeType)
	{
		if (pathType == EPathType::File)
		{
			Config::Load();
		}
	};
	_fileWatch->StartWatching();
}

Config::~Config() noexcept
{
}

void Config::Save() noexcept
{
	try
	{
		std::string f = AppConfigFile;
		std::ofstream out{f};
		if (out.is_open())
		{
			out << _config.dump(1, '\t');
			out.close();
			TE_TRACE("Config file saved: \"{}\"", f);
		}
		else
		{
			TE_ERROR("Failed to open config file for writing: \"{}\"", f);
		}
	}
	catch (const std::exception &e)
	{
		TE_ERROR("Exception occurred while saving config file: {}", e.what());
	}
}

void Config::Load() noexcept
{
	try
	{
		std::ifstream in{AppConfigFile};
		if (in.is_open())
		{
			_config = nlohmann::json::parse(in);
			in.close();
		}
		else
		{
			_config = nlohmann::json();
			std::ofstream out{std::string(AppConfigFile)};
			out << _config.dump(4);
			out.close();
		}
		Log::UpdateVerbosities(&::GetLog(_config));
	}
	catch (const std::exception &e)
	{
		TE_ERROR("Exception occurred while loading config file: {}", e.what());
	}
}

std::vector<std::string> Config::GetMountBitmaps() const noexcept
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

std::vector<std::string> Config::GetMountDirectories() const noexcept
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

std::unordered_map<std::string, EResourceType> Config::GetMountFiles() const noexcept
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

bool Config::GetRenderBackend() const noexcept
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

std::uint32_t Config::GetWindowFramelimit() const noexcept
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

std::string_view Config::GetWindowTitle() const noexcept
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

std::uint32_t Config::GetWindowWidth() const noexcept
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

std::uint32_t Config::GetWindowHeight() const noexcept
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

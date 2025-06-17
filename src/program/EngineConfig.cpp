#include "EngineConfig.h"

#include "graphic/ERenderBackend.h"
#include "util/Defs.h"
#include "util/Log.h"

#include <json.hpp>

#include <fstream>

using namespace tudov;

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

static const auto valueRenderBackend = ERenderBackend::SDL;
static const auto valueWindowFramelimit = 60;
static const auto valueWindowFullscreen = false;
static const auto valueWindowHeight = 720;
static const auto valueWindowTitle = "DR2C Together";
static const auto valueWindowWidth = 1280;
static const auto valueMountDirectories = std::vector<std::string>{
    "data",
    "gfx",
};
static const auto valueMountFiles = std::unordered_map<std::string, ResourceType>{
    {".png", ResourceType::Texture},
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

EngineConfig::EngineConfig() noexcept
    : _log(Log::Get("EngineConfig"))
{
	EngineConfig::Load();

	_fileWatcher = std::make_unique<filewatch::FileWatch<std::string>>(std::string(file), [&](std::string_view path, const filewatch::Event changeType)
	{
		EngineConfig::Load();
	});
}

EngineConfig::~EngineConfig() noexcept
{
	Save();
}

void EngineConfig::Save() noexcept
{
	try
	{
		std::string f{file};
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

void EngineConfig::Load() noexcept
{
	try
	{
		std::ifstream f{std::string(file)};
		if (f.is_open())
		{
			_config = nlohmann::json::parse(f);
			f.close();
		}
		else
		{
			_config = nlohmann::json();
			std::ofstream out{std::string(file)};
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

std::vector<std::string> EngineConfig::GetMountDirectories() noexcept
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

std::unordered_map<std::string, ResourceType> EngineConfig::GetMountFiles() noexcept
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

ERenderBackend EngineConfig::GetRenderBackend() noexcept
{
	auto &&renderBackend = _config[keyRenderBackend];
	if (!renderBackend.is_string())
	{
		renderBackend = valueRenderBackend;
		_config[keyRenderBackend] = renderBackend;
	}
	return renderBackend;
}

std::uint32_t EngineConfig::GetWindowFramelimit() noexcept
{
	auto &&window = GetWindow(_config);
	auto &&framelimit = window[keyFramelimit];
	if (!framelimit.is_string())
	{
		framelimit = valueWindowFramelimit;
		window[keyFramelimit] = framelimit;
	}
	return framelimit;
}

std::string_view EngineConfig::GetWindowTitle() noexcept
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

std::uint32_t EngineConfig::GetWindowWidth() noexcept
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

std::uint32_t EngineConfig::GetWindowHeight() noexcept
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

void EngineConfig::SetWindowTitle(const std::string &) noexcept
{
}

void EngineConfig::SetWindowWidth(std::uint32_t) noexcept
{
}

void EngineConfig::SetWindowHeight(std::uint32_t) noexcept
{
}

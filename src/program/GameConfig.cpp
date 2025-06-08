#include "GameConfig.h"

#include <fstream>

using namespace tudov;

GameConfig::GameConfig()
{
}

GameConfig::~GameConfig()
{
	Save();
}

void GameConfig::Save()
{
}

void GameConfig::Load()
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
}

nlohmann::json GetWindow(nlohmann::json &config)
{
	auto &&window = config["window"];
	if (!window.is_object())
	{
		window = {
		    {"width", 1280},
		    {"height", 720},
		    {"fullscreen", false},
		};
		config["window"] = window;
	}
	return window;
}

std::string GameConfig::GetWindowTitle()
{
	auto &&width = GetWindow(_config)["width"];
	if (width.is_string())
	{
		return std::string(width);
	}
	return std::string("Death Road to Canada");
}

uint32_t tudov::GameConfig::GetWindowWidth()
{
	return 0;
}

uint32_t tudov::GameConfig::GetWindowHeight()
{
	return 0;
}

void tudov::GameConfig::SetWindowTitle(const std::string &)
{
}

void tudov::GameConfig::SetWindowWidth(uint32_t)
{
}

void tudov::GameConfig::SetWindowHeight(uint32_t)
{
}

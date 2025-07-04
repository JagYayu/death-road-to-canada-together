#include "UnpackagedMod.h"

#include "Mod.h"
#include "ModConfig.hpp"
#include "ModManager.h"
#include "graphic/sdl/SDLTrueTypeFont.h"
#include "program/Engine.h"
#include "util/StringUtils.hpp"

#include <FileWatch.hpp>
#include <filesystem>
#include <format>
#include <memory>
#include <regex>
#include <string_view>
#include <vector>

using namespace tudov;

const std::filesystem::path &file = "Mod.json";

bool UnpackagedMod::IsValidDirectory(const std::filesystem::path &directory)
{
	return std::filesystem::exists(directory / file);
}

ModConfig UnpackagedMod::LoadConfig(const std::filesystem::path &directory)
{
	auto &&path = directory / file;
	std::ifstream file{path};
	if (!file)
	{
		return ModConfig();
	}

	nlohmann::json json;
	file >> json;
	ModConfig config;
	try
	{
		config = json.get<tudov::ModConfig>();
	}
	catch (...)
	{
		Log::Get("UnpackagedMod")->Error("Error parsing mod config file at: {}", path.string());
		config = ModConfig();
	}
	file.close();
	return config;
}

UnpackagedMod::UnpackagedMod(ModManager &modManager, const std::filesystem::path &directory)
    : Mod(modManager, LoadConfig(directory)),
      log(Log::Get("UnpackagedMod")),
      _directory(directory)
{
}

void UnpackagedMod::UpdateFilePatterns()
{
	_scriptFilePatterns = std::vector<std::regex>(_config.scripts.files.size());
	for (auto &&pattern : _config.scripts.files)
	{
		_scriptFilePatterns.emplace_back(std::regex(std::string(pattern), std::regex_constants::icase));
	}

	_fontFilePatterns = std::vector<std::regex>(_config.fonts.files.size());
	for (auto &&pattern : _config.fonts.files)
	{
		_fontFilePatterns.emplace_back(std::regex(std::string(pattern), std::regex_constants::icase));
	}
}

inline bool RegexPatternMatch(std::string_view file, const std::vector<std::regex> &patterns) noexcept
{
	for (auto &&pattern : patterns)
	{
		if (std::regex_match(file.data(), pattern))
		{
			return true;
		}
	}
	return false;
}

bool UnpackagedMod::IsScript(std::string_view file) const
{
	return RegexPatternMatch(file, _scriptFilePatterns);
}

bool UnpackagedMod::IsFont(std::string_view file) const
{
	return RegexPatternMatch(file, _fontFilePatterns);
}

void UnpackagedMod::Load()
{
	if (_fileWatcher)
	{
		log->Warn("Mod has already loaded");
		return;
	}

	_fileWatcher = std::make_shared<filewatch::FileWatch<std::string>>(_directory.string(), [&](const std::string &filePath, const filewatch::Event changeType)
	{
		auto &&scriptProvider = modManager.scriptProvider;

		if (IsScript(filePath))
		{
			switch (changeType)
			{
			case filewatch::Event::added:
				break;
			case filewatch::Event::removed:
				break;
			case filewatch::Event::modified:
			{
				log->Debug("Script modified");

				auto &&relative = std::filesystem::relative(filePath, GetScriptsDirectory());
				auto &&scriptName = FilePathToLuaScriptName(std::format("{}.{}", _config.namespace_, relative.string()));

				std::ifstream ins{_directory / filePath};
				std::ostringstream oss;
				oss << ins.rdbuf();
				ins.close();

				modManager.HotReloadScriptPending(scriptName, oss.str());

				break;
			}
			case filewatch::Event::renamed_old:
			case filewatch::Event::renamed_new:
				break;
			}
		}
	});

	auto &&namespace_ = GetNamespace();
	if (namespace_.empty())
	{
		log->Debug("Cannot load unpacked mod at \"{}\", invalid namespace \"{}\"", _directory.string(), std::string_view(namespace_));
		return;
	}

	auto &&dir = _directory.string();

	log->Debug("Loading unpacked mod from \"{}\" ...", dir);

	UpdateFilePatterns();

	for (const auto &entry : std::filesystem::recursive_directory_iterator(_directory))
	{
		if (!entry.is_regular_file())
		{
			continue;
		}

		auto &&filePath = entry.path().generic_string();
		// auto &&fileName = filePath.filename().generic_string();
		if (IsScript(filePath))
		{
			std::ifstream ins{filePath};
			std::ostringstream oss;
			oss << ins.rdbuf();
			ins.close();

			auto &&relative = std::filesystem::relative(std::filesystem::relative(filePath, _directory), GetScriptsDirectory());
			auto &&scriptName = FilePathToLuaScriptName(std::format("{}.{}", namespace_, relative.string()));
			auto &&scriptID = modManager.scriptProvider.AddScript(scriptName, oss.str(), namespace_);
			_scripts.emplace_back(scriptID);
		}
		else if (IsFont(filePath))
		{
			auto fontID = modManager.engine.fontManager.Load<SDLTrueTypeFont>(filePath, ReadFileToString(filePath, true));
			_fonts.emplace_back(fontID);
		}
	}

	log->Debug("Loaded unpacked mod from \"{}\"", dir);
}

void UnpackagedMod::Unload()
{
	if (!_fileWatcher)
	{
		log->Warn("Mod has not been loaded");
		return;
	}

	auto &&dir = _directory.string();

	log->Debug("Unloading unpackaged mod from \"{}\"", dir);

	for (auto &&scriptID : _scripts)
	{
		modManager.scriptProvider.RemoveScript(scriptID);
	}
	_scripts.clear();

	for (auto &&fontID : _fonts)
	{
		modManager.engine.fontManager.Unload(fontID);
	}
	_fonts.clear();

	log->Debug("Unloaded unpackaged mod \"{}\"", dir);
}

#include "UnpackagedMod.h"

#include "Mod.h"
#include "ModManager.h"
#include "util/StringUtils.hpp"

#include <filesystem>
#include <format>
#include <regex>
#include <string>

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
	tudov::ModConfig config = json.get<tudov::ModConfig>();
	file.close();
	return config;
}

UnpackagedMod::UnpackagedMod(ModManager &modManager, const std::filesystem::path &directory)
    : Mod(modManager, LoadConfig(directory)),
      _log(Log::Get("UnpackagedMod")),
      _directory(directory)
{
}

void UnpackagedMod::Load()
{
	_watcher = MakeShared<filewatch::FileWatch<String>>(_directory.string(), [](const String &path, const filewatch::Event changeType)
	{
		std::wcout << std::filesystem::absolute(path) << L"\n";
	});

	auto &&namespace_ = GetNamespace();
	if (namespace_.empty())
	{
		_log->Debug(Format("Cannot load unpacked mod at \"{}\", invalid namespace \"{}\"", _directory.string(), StringView(namespace_)));
		return;
	}

	auto &&dir = _directory.string();

	_log->Debug(Format("Loading unpacked mod from \"{}\" ...", dir));

	auto &&scriptFilePatterns = std::vector<std::regex>(_config.scripts.files.size());
	for (auto &&pattern : _config.scripts.files)
	{
		scriptFilePatterns.emplace_back(std::regex(String(pattern), std::regex_constants::icase));
	}

	for (const auto &entry : std::filesystem::recursive_directory_iterator(_directory))
	{
		if (!entry.is_regular_file())
		{
			continue;
		}

		auto &&file = entry.path();
		auto &&fileName = file.filename().string();
		for (auto &&pattern : scriptFilePatterns)
		{
			if (std::regex_match(fileName, pattern))
			{
				std::ifstream ins{file};
				std::ostringstream oss;
				oss << ins.rdbuf();
				ins.close();

				auto &&relative = std::filesystem::relative(std::filesystem::relative(file, _directory), GetScriptsDirectory());
				auto &&scriptName = FilePathToLuaScriptName(Format("{}.{}", namespace_, relative.string()));
				_scripts.emplace_back(scriptName);
				modManager.scriptProvider.AddScript(scriptName, oss.str());
			}
		}
	}

	_log->Debug(Format("Loaded unpacked mod from \"{}\"", dir));
}

void UnpackagedMod::Unload()
{
	auto &&dir = _directory.string();

	_log->Debug(Format("Unloading unpackaged mod from \"{}\"", dir));

	for (auto &&scriptName : _scripts)
	{
		modManager.scriptProvider.RemoveScript(scriptName);
	}

	_log->Debug(Format("Unloaded unpackaged mod \"{}\"", dir));
}

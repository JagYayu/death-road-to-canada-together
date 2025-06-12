#include "UnpackagedMod.h"

#include "Mod.h"
#include "ModManager.h"
#include "lib/FileWatch.hpp"
#include "util/StringUtils.hpp"

#include <filesystem>
#include <format>
#include <regex>

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
      log(Log::Get("UnpackagedMod")),
      _directory(directory)
{
}

void UnpackagedMod::UpdateFilePatterns()
{
	_scriptFilePatterns = Vector<std::regex>(_config.scripts.files.size());
	for (auto &&pattern : _config.scripts.files)
	{
		_scriptFilePatterns.emplace_back(std::regex(String(pattern), std::regex_constants::icase));
	}
}

bool UnpackagedMod::IsScript(const String &fileName) const
{
	for (auto &&pattern : _scriptFilePatterns)
	{
		if (std::regex_match(fileName, pattern))
		{
			return true;
		}
	}
	return false;
}

void UnpackagedMod::Load()
{
	if (_fileWatcher)
	{
		log->Warn("Mod has already loaded");
		return;
	}

	_fileWatcher = MakeShared<filewatch::FileWatch<String>>(_directory.string(), [&](const String &filePath, const filewatch::Event changeType)
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
				auto &&scriptName = FilePathToLuaScriptName(Format("{}.{}", _config.namespace_, relative.string()));
				// auto &&scriptID = scriptProvider.GetScriptIDByName(scriptName);
				// auto &&scriptLoader = modManager.scriptEngine.scriptLoader;

				// auto &&pendingScripts = scriptLoader.Unload(scriptID);
				// scriptProvider.RemoveScript(scriptID);

				std::ifstream ins{_directory / filePath};
				std::ostringstream oss;
				oss << ins.rdbuf();
				ins.close();

				// scriptID = scriptProvider.AddScript(scriptName, oss.str());
				// pendingScripts.emplace_back(scriptID);
				// scriptLoader.HotReload(pendingScripts);

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
		log->Debug("Cannot load unpacked mod at \"{}\", invalid namespace \"{}\"", _directory.string(), StringView(namespace_));
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

		auto &&file = entry.path();
		auto &&fileName = file.filename().string();
		if (IsScript(fileName))
		{
			std::ifstream ins{file};
			std::ostringstream oss;
			oss << ins.rdbuf();
			ins.close();

			auto &&relative = std::filesystem::relative(std::filesystem::relative(file, _directory), GetScriptsDirectory());
			auto &&scriptName = FilePathToLuaScriptName(Format("{}.{}", namespace_, relative.string()));
			auto &&scriptID = modManager.scriptProvider.AddScript(scriptName, oss.str());
			_scripts.emplace_back(scriptID);
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

	log->Debug("Unloaded unpackaged mod \"{}\"", dir);
}

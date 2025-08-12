#include "mod/UnpackagedMod.hpp"

#include "data/VirtualFileSystem.hpp"
#include "mod/Mod.hpp"
#include "mod/ModConfig.hpp"
#include "mod/ModManager.hpp"
#include "mod/ScriptLoader.hpp"
#include "resource/FontResources.hpp"
#include "resource/GlobalResourcesCollection.hpp"
#include "resource/Text.hpp"
#include "resource/TextResources.hpp"
#include "util/StringUtils.hpp"

#include <cassert>
#include <filesystem>
#include <format>
#include <memory>
#include <regex>
#include <string_view>
#include <vector>

using namespace tudov;

const std::filesystem::path &modConfigFile = "Mod.json";

UnpackagedMod::UnpackagedMod(ModManager &modManager, const std::filesystem::path &directory)
    : Mod(modManager, LoadConfig(directory)),
      _log(Log::Get("UnpackagedMod")),
      _loaded(false),
      _directory(directory)
{
}

Context &UnpackagedMod::GetContext() noexcept
{
	return _modManager.GetContext();
}

std::filesystem::path UnpackagedMod::GetDirectory() noexcept
{
	return _directory;
}

void UnpackagedMod::UpdateFileMatchPatterns()
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

bool UnpackagedMod::IsValidDirectory(const std::filesystem::path &directory)
{
	return std::filesystem::exists(directory / modConfigFile);
}

ModConfig UnpackagedMod::LoadConfig(const std::filesystem::path &directory)
{
	auto &&path = directory / modConfigFile;
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
		Log::Get("UnpackagedMod")->Error("Error parsing mod config file at: {}", path.generic_string());
		config = ModConfig();
	}
	file.close();
	return config;
}

void UnpackagedMod::Load()
{
	if (_fileWatcher)
	{
		_log->Warn("Mod has already loaded");
		return;
	}

	_fileWatcher = std::make_unique<filewatch::FileWatch<std::string>>(_directory.generic_string(), [this](const std::filesystem::path &filePath, const filewatch::Event changeType)
	{
		// auto &&scriptProvider = _modManager.GetScriptProvider();

		std::string file = (_directory / filePath).generic_string();
		if (IsScript(file))
		{
			_log->Trace("Script modified: \"{}\"", file);

			switch (changeType)
			{
			case filewatch::Event::added:
				ScriptAdded(file);
				break;
			case filewatch::Event::removed:
				ScriptRemoved(file);
				break;
			case filewatch::Event::modified:
				ScriptModified(file);
				break;
			case filewatch::Event::renamed_old:
				break;
			case filewatch::Event::renamed_new:
				break;
			default:
				break;
			}
		}
	});

	auto &&namespace_ = IUnpackagedMod::GetNamespace();
	if (namespace_.empty())
	{
		_log->Debug("Cannot load unpacked mod at \"{}\", invalid namespace \"{}\"", _directory.generic_string(), std::string_view(namespace_));
		return;
	}

	auto &&dir = _directory.generic_string();

	_log->Debug("Loading unpacked mod from \"{}\" ...", dir);

	UpdateFileMatchPatterns();

	ModConfig &config = Mod::GetConfig();

	// auto &&scriptProvider = _modManager.GetScriptProvider();
	for (const auto &entry : std::filesystem::recursive_directory_iterator(_directory))
	{
		if (!entry.is_regular_file())
		{
			continue;
		}

		auto &&file = entry.path().generic_string();
		if (IsScript(file))
		{
			ScriptAdded(file);
		}
		// else if (IsFont(filePath))
		// {
		// 	auto fontID = modManager.engine.fontResources.Load<SDLTrueTypeFont>(filePath, ReadFileToString(filePath, true));
		// 	_fonts.emplace_back(fontID);
		// }
	}

	_log->Debug("Loaded unpacked mod from \"{}\"", dir);

	_loaded = true;
}

void UnpackagedMod::ScriptAdded(const std::filesystem::path &file) noexcept
{
	std::filesystem::path &&relative = std::filesystem::relative(file, _directory);
	relative = std::filesystem::relative(relative, _config.scripts.directory);
	std::string filePathStr = file.generic_string();
	std::string relativePath = relative.generic_string();
	std::string scriptName = FilePathToLuaScriptName(std::format("{}.{}", _config.namespace_, relativePath));
	std::vector<std::byte> scriptCode = ReadFileToBytes(filePathStr, true);

	GetVirtualFileSystem().MountFile(file, scriptCode);

	TextResources &textResources = GetGlobalResourcesCollection().GetTextResources();
	TextID scriptTextID = textResources.GetResourceID(filePathStr);
	assert(scriptTextID != 0 && "It looks like texture resource is not loaded after file mounting.");

	if (ShouldScriptLoad(relativePath))
	{
		_modManager.UpdateScriptPending(scriptName, scriptTextID, _config.uniqueID);
	}
}

void UnpackagedMod::ScriptRemoved(const std::filesystem::path &file) noexcept
{
	std::filesystem::path &&relative = std::filesystem::relative(file, _directory);
	relative = std::filesystem::relative(relative, _config.scripts.directory);

	std::string scriptName = FilePathToLuaScriptName(std::format("{}.{}", _config.namespace_, relative.generic_string()));

	assert(GetVirtualFileSystem().DismountFile(file) && "Invalid file path!");

	_modManager.UpdateScriptPending(scriptName, 0, _config.uniqueID);
}

void UnpackagedMod::ScriptModified(const std::filesystem::path &file) noexcept
{
	std::filesystem::path &&relative = std::filesystem::relative(file, _directory);
	relative = std::filesystem::relative(relative, _config.scripts.directory);
	std::string scriptName = FilePathToLuaScriptName(std::format("{}.{}", _config.namespace_, relative.generic_string()));

	std::string filePathStr = file.generic_string();
	std::string relativePath = relative.generic_string();
	std::vector<std::byte> scriptCode = ReadFileToBytes(filePathStr, true);

	assert(GetVirtualFileSystem().RemountFile(file, scriptCode) && "Invalid file path!");

	TextResources &textResources = GetGlobalResourcesCollection().GetTextResources();
	TextID scriptTextID = textResources.GetResourceID(filePathStr);
	assert(scriptTextID != 0 && "It looks like texture resource is not loaded after file mounting.");

	if (ShouldScriptLoad(relativePath))
	{
		_modManager.UpdateScriptPending(scriptName, scriptTextID, _config.uniqueID);
	}
	else
	{
		_modManager.UpdateScriptPending(scriptName, 0, _config.uniqueID);
	}
}

void UnpackagedMod::Unload()
{
	if (!_loaded)
	{
		_log->Warn("Mod has not been loaded");
		return;
	}

	auto &&dir = _directory.generic_string();

	_log->Debug("Unloading unpackaged mod from \"{}\"", dir);

	{
		auto &modUID = Mod::GetConfig().uniqueID;
		_modManager.GetScriptLoader().UnloadScriptsBy(modUID);
		_modManager.GetScriptProvider().RemoveScriptsBy(modUID);
		// _scripts.clear();
	}

	for (auto &&fontID : _fonts)
	{
		_modManager.GetGlobalResourcesCollection().GetFontResources().Unload(fontID);
	}
	_fonts.clear();

	_log->Debug("Unloaded unpackaged mod \"{}\"", dir);
}

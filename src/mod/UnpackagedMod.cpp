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
		auto &&scriptProvider = _modManager.GetScriptProvider();

		auto file = filePath.generic_string();
		if (IsScript(file))
		{
			switch (changeType)
			{
			case filewatch::Event::added:
				break;
			case filewatch::Event::removed:
				break;
			case filewatch::Event::modified:
			{
				_log->Trace("Script modified: \"{}\"", file);

				auto &&relative = std::filesystem::relative(file, _config.scripts.directory);
				auto &&scriptName = FilePathToLuaScriptName(std::format("{}.{}", _config.namespace_, relative.generic_string()));

				{
					std::ifstream ins{_directory / filePath};
					std::ostringstream oss;
					oss << ins.rdbuf();
					ins.close();

					auto str = oss.str();
					const auto *bytes = reinterpret_cast<const std::byte *>(str.data());
					GetVirtualFileSystem().RemountFile(filePath, std::vector<std::byte>(bytes, bytes + str.size()));
				}

				auto textResources = GetGlobalResourcesCollection().GetTextResources();
				ResourceID resourceID = textResources.GetResourceID(file);
				assert(resourceID);

				_modManager.HotReloadScriptPending(scriptName, textResources.GetResource(resourceID), _config.uniqueID);

				break;
			}
			case filewatch::Event::renamed_old:
			case filewatch::Event::renamed_new:
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

	UpdateFilePatterns();

	ModConfig &config = Mod::GetConfig();

	auto &&scriptProvider = _modManager.GetScriptProvider();
	for (const auto &entry : std::filesystem::recursive_directory_iterator(_directory))
	{
		if (!entry.is_regular_file())
		{
			continue;
		}

		auto &&filePath = entry.path().generic_string();
		if (IsScript(filePath))
		{
			auto &scripts = config.scripts;

			auto &&relativePath = std::filesystem::relative(std::filesystem::relative(filePath, _directory), scripts.directory).generic_string();
			auto &&scriptName = FilePathToLuaScriptName(std::format("{}.{}", namespace_, relativePath));
			std::string scriptCode;
			{
				std::ifstream ins{filePath};
				std::ostringstream oss;
				oss << ins.rdbuf();
				ins.close();
				scriptCode = oss.str();
			}

			auto &textResources = GetGlobalResourcesCollection().GetTextResources();
			auto textID = textResources.Load<TextResource>(filePath, scriptCode);
			assert(textID != 0);

			if (ShouldScriptLoad(relativePath))
			{
				auto scriptCode = textResources.GetResource(textID);
				assert(scriptCode != nullptr);
				scriptProvider.AddScript(scriptName, scriptCode, _config.uniqueID);
			}
			// _scripts.emplace_back(scriptName);
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
		_modManager.GetScriptProvider().RemoveScriptBy(modUID);
		// _scripts.clear();
	}

	for (auto &&fontID : _fonts)
	{
		_modManager.GetGlobalResourcesCollection().GetFontResources().Unload(fontID);
	}
	_fonts.clear();

	_log->Debug("Unloaded unpackaged mod \"{}\"", dir);
}

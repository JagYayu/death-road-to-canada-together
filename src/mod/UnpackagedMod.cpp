/**
 * @file mod/UnpackagedMod.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "mod/UnpackagedMod.hpp"

#include "data/PathType.hpp"
#include "data/VirtualFileSystem.hpp"
#include "mod/Mod.hpp"
#include "mod/ModConfig.hpp"
#include "mod/ModManager.hpp"
#include "mod/ScriptLoader.hpp"
#include "program/Engine.hpp"
#include "resource/FontResources.hpp"
#include "resource/GlobalResourcesCollection.hpp"
#include "resource/TextResources.hpp"
#include "util/FileChangeType.hpp"
#include "util/FileSystemWatch.hpp"
#include "util/LogMicros.hpp"
#include "util/StringUtils.hpp"


#include <filesystem>
#include <format>
#include <memory>
#include <regex>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

using namespace tudov;

const std::filesystem::path &modConfigFile = "Mod.json";

UnpackagedMod::UnpackagedMod(ModManager &modManager, const std::filesystem::path &directory)
    : Mod(modManager, LoadConfig(directory)),
      _log(Log::Get("UnpackagedMod")),
      _loaded(false),
      _directory(directory),
      _fileWatcher(nullptr)
{
}

UnpackagedMod::~UnpackagedMod() noexcept
{
}

Context &UnpackagedMod::GetContext() noexcept
{
	return _modManager.GetContext();
}

Log &UnpackagedMod::GetLog() noexcept
{
	return *_log;
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

void UnpackagedMod::Update()
{
	std::lock_guard lock{_fileWatchMutex};

	while (!_fileWatchQueue.empty()) [[unlikely]]
	{
		auto [path, type, change] = _fileWatchQueue.front();
		_fileWatchQueue.pop();

		std::string file = (_directory / path).generic_string();
		switch (change)
		{
		case EFileChangeType::Added:
		case EFileChangeType::RenamedNew:
		{
			if (type == EPathType::File && IsScript(file))
			{
				ScriptAdded(file);
			}
			else
			{
				switch (type)
				{
				case EPathType::File:
					FileAdded(file);
					break;
				case EPathType::Directory:
					// DirectoryAdded(file);
					break;
				default:
					break;
				}
			}
			break;
		}
		case EFileChangeType::Removed:
		{
			if (type == EPathType::File && IsScript(file))
			{
				ScriptRemoved(file);
			}
			else
			{
				switch (type)
				{
				case EPathType::File:
					FileRemoved(file);
					break;
				case EPathType::Directory:
					break;
				default:
					break;
				}
			}
		}
		case EFileChangeType::RenamedOld:
		{
			if (IsScript(file))
			{
				ScriptRemoved(file);
			}
			else
			{
				FileRemoved(file);
			}
			break;
		}
		case EFileChangeType::Modified:
		{
			if (type == EPathType::File && IsScript(file))
			{
				ScriptModified(file);
			}
			else
			{
				switch (type)
				{
				case EPathType::File:
					FileModified(file);
					break;
				case EPathType::Directory:
					break;
				default:
					break;
				}
			}
			break;
		}
		[[unlikely]]
		default:
			TE_WARN("Unexpected file change type: {}", static_cast<std::underlying_type_t<EFileChangeType>>(change));
			break;
		}

		// if (change == EFileChangeType::RenamedOld)
		// {
		// 	TE_DEBUG("File renamed old: \"{}\"", file);
		// 	if (!ScriptRemoved(file))
		// 	{
		// 		FileRemoved(file);
		// 	}
		// }
		// else if (type == EPathType::File)
		// {
		// 	if (IsScript(file))
		// 	{
		// 		switch (change)
		// 		{
		// 		case EFileChangeType::Added:
		// 			TE_DEBUG("Script added: \"{}\"", file);
		// 			ScriptAdded(file);
		// 			break;
		// 		case EFileChangeType::Removed:
		// 			TE_DEBUG("Script removed: \"{}\"", file);
		// 			ScriptRemoved(file);
		// 			break;
		// 		case EFileChangeType::Modified:
		// 			TE_DEBUG("Script modified: \"{}\"", file);
		// 			ScriptModified(file);
		// 			break;
		// 		case EFileChangeType::RenamedOld:
		// 		case EFileChangeType::RenamedNew:
		// 			TE_DEBUG("Script renamed new: \"{}\"", file);
		// 			ScriptAdded(file);
		// 			break;
		// 		[[unlikely]]
		// 		default:
		// 			TE_WARN("Script unknown event: \"{}\"", file);
		// 			break;
		// 		}
		// 	}
		// 	else
		// 	{
		// 		switch (change)
		// 		{
		// 		case EFileChangeType::Added:
		// 			TE_DEBUG("File added: \"{}\"", file);
		// 			FileAdded(file);
		// 			break;
		// 		case EFileChangeType::Removed:
		// 			TE_DEBUG("File removed: \"{}\"", file);
		// 			FileRemoved(file);
		// 			break;
		// 		case EFileChangeType::Modified:
		// 			TE_DEBUG("File modified: \"{}\"", file);
		// 			FileModified(file);
		// 			break;
		// 		case EFileChangeType::RenamedOld:
		// 			TE_DEBUG("File renamed old: \"{}\"", file);
		// 			FileRemoved(file);
		// 			break;
		// 		case EFileChangeType::RenamedNew:
		// 			TE_DEBUG("File renamed new: \"{}\"", file);
		// 			FileAdded(file);
		// 			break;
		// 		default:
		// 			TE_WARN("File unknown event: \"{}\"", file);
		// 			break;
		// 		}
		// 	}
		// }
	}
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
		TE_WARN("{}", "Unpackaged mod has already loaded");

		return;
	}

	_fileWatcher = std::make_unique<FileSystemWatch>(_directory.generic_string());
	_fileWatcher->GetOnCallback() += [this](const std::filesystem::path &filePath, EPathType pathType, EFileChangeType changeType)
	{
		if (_loaded)
		{
			std::lock_guard lock{_fileWatchMutex};

			_fileWatchQueue.push(std::make_tuple(filePath, pathType, changeType));

			GetEngine().GetLoadingState().store(Engine::ELoadingState::Pending);
		}
	};
	_fileWatcher->StartWatching();

	auto &&namespace_ = IUnpackagedMod::GetNamespace();
	if (namespace_.empty())
	{
		TE_DEBUG("Cannot load unpacked mod at \"{}\", invalid namespace \"{}\"", _directory.generic_string(), std::string_view(namespace_));
		return;
	}

	const std::string &dir = _directory.generic_string();

	TE_DEBUG("Loading unpacked mod from \"{}\" ...", dir);

	UpdateFileMatchPatterns();

	ModConfig &config = GetConfig();

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
		else
		{
			FileAdded(file);
		}
	}

	TE_DEBUG("Loaded unpacked mod from \"{}\"", dir);

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
	TE_ASSERT(scriptTextID != 0 && "It looks like texture resource is not loaded after file mounting.");

	if (ShouldScriptLoad(relativePath))
	{
		_modManager.UpdateScriptPending(scriptName, scriptTextID, _config.uid);
	}
}

void UnpackagedMod::FileAdded(const std::filesystem::path &file) noexcept
{
	GetVirtualFileSystem().MountFile(file, ReadFileToBytes(file.generic_string(), true));
}

bool UnpackagedMod::ScriptRemoved(const std::filesystem::path &file) noexcept
{
	std::filesystem::path &&relative = std::filesystem::relative(file, _directory);
	relative = std::filesystem::relative(relative, _config.scripts.directory);

	std::string scriptName = FilePathToLuaScriptName(std::format("{}.{}", _config.namespace_, relative.generic_string()));

	if (!GetVirtualFileSystem().DismountFile(file))
	{
		return false;
	}

	_modManager.UpdateScriptPending(scriptName, 0, _config.uid);

	return true;
}

void UnpackagedMod::FileRemoved(const std::filesystem::path &file) noexcept
{
	GetVirtualFileSystem().DismountFile(file);
}

bool UnpackagedMod::ScriptModified(const std::filesystem::path &file) noexcept
{
	std::string filePathStr = file.generic_string();
	if (!std::filesystem::exists(filePathStr))
	{
		return false;
	}

	std::vector<std::byte> scriptCode = ReadFileToBytes(filePathStr, true);

	if (!GetVirtualFileSystem().RemountFile(file, scriptCode))
	{
		return false;
	}

	std::filesystem::path &&relative = std::filesystem::relative(file, _directory);
	relative = std::filesystem::relative(relative, _config.scripts.directory);

	std::string scriptName = FilePathToLuaScriptName(std::format("{}.{}", _config.namespace_, relative.generic_string()));
	std::string relativePath = relative.generic_string();

	TextResources &textResources = GetGlobalResourcesCollection().GetTextResources();
	TextID scriptTextID = textResources.GetResourceID(filePathStr);
	TE_ASSERT(scriptTextID != 0 && "It looks like texture resource is not loaded after file mounting.");

	if (ShouldScriptLoad(relativePath))
	{
		_modManager.UpdateScriptPending(scriptName, scriptTextID, _config.uid);
	}
	else
	{
		_modManager.UpdateScriptPending(scriptName, 0, _config.uid);
	}

	return true;
}

void UnpackagedMod::FileModified(const std::filesystem::path &file) noexcept
{
	GetVirtualFileSystem().RemountFile(file, ReadFileToBytes(file.generic_string(), true));
}

void UnpackagedMod::Unload()
{
	if (!_loaded)
	{
		TE_WARN("{}", "Mod has not been loaded");
		return;
	}

	const std::string &dir = _directory.generic_string();

	TE_DEBUG("Unloading unpackaged mod from \"{}\"", dir);

	{
		auto &modUID = Mod::GetConfig().uid;
		GetScriptLoader().UnloadScriptsBy(modUID);
		GetScriptProvider().RemoveScriptsBy(modUID);
		GetVirtualFileSystem().DismountDirectory(_directory);
	}

	for (auto &&fontID : _fonts)
	{
		GetGlobalResourcesCollection().GetFontResources().Unload(fontID);
		// todo use this `GetGlobalResourcesCollection().GetFontResources().UnloadAllFromDirectory`?
	}
	_fonts.clear();

	TE_DEBUG("Unloaded unpackaged mod from \"{}\"", dir);
}

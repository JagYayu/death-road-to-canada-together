/**
 * @file mod/ScriptProvider.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "mod/ScriptProvider.hpp"

#include "data/VirtualFileSystem.hpp"
#include "resource/GlobalResourcesCollection.hpp"
#include "system/LogMicros.hpp"
#include "util/Definitions.hpp"
#include "util/EnumFlag.hpp"
#include "util/StringUtils.hpp"

#include <stdexcept>
#include <string_view>
#include <unordered_set>

using namespace tudov;

static constexpr const char *scriptPrefix = "#";
static constexpr const char *scriptDirectory = "lua";
static constexpr const char *scriptNamespace = "#lua";
static constexpr const char *scriptNamespacePrefix = "#lua.";

std::string StaticScriptNamespace(const std::string &path)
{
	return scriptPrefix + FilePathToLuaScriptName(path);
}

ScriptProvider::ScriptProvider(Context &context) noexcept
    : _context(context),
      _log(Log::Get("ScriptProvider")),
      _versionID(0),
      _latestScriptID(0)
{
}

ScriptProvider::~ScriptProvider()
{
}

Context &ScriptProvider::GetContext() noexcept
{
	return _context;
}

Log &ScriptProvider::GetLog() noexcept
{
	return *_log;
}

void ScriptProvider::Initialize() noexcept
{
	constexpr decltype(auto) directory = "app/lua";

	auto entries = GetVirtualFileSystem().List(directory, EnumFlag::BitOr(
	                                                          EPathListOption::File,
	                                                          EPathListOption::Recursed,
	                                                          EPathListOption::Sorted));
	TextResources &textResources = GetGlobalResourcesCollection().GetTextResources();

	std::unordered_set<std::string_view> excludedScripts = {
	    "jit/bc.lua",
	    "jit/bcsave.lua",
	    "jit/dis_arm.lua",
	    "jit/dis_arm64.lua",
	    "jit/dis_arm64be.lua",
	    "jit/dis_mips.lua",
	    "jit/dis_mips64.lua",
	    "jit/dis_mips64el.lua",
	    "jit/dis_mips64r6.lua",
	    "jit/dis_mips64r6el.lua",
	    "jit/dis_mipsel.lua",
	    "jit/dis_ppc.lua",
	    "jit/dis_x64.lua",
	    "jit/dis_x86.lua",
	    "jit/dump.lua",
	    "jit/p.lua",
	    "jit/v.lua",
	    "jit/zone.lua",
	    "lulpeg.lua",
	};

	for (auto &&[relativePath, isDirectory] : entries)
	{
		if (excludedScripts.contains(relativePath))
		{
			continue;
		}

		std::filesystem::path relative = relativePath;
		if (relative.extension() != ".lua")
		{
			continue;
		}

		auto &&fullPath = (directory / relative).generic_string();
		TextID textID = textResources.GetResourceID(fullPath);

		if (textID != 0) [[likely]]
		{
			auto &&scriptName = StaticScriptNamespace(std::string(relativePath));
			// TE_ASSERT(code != nullptr);

			AddScriptImpl(scriptName, textID);
		}
		else [[unlikely]]
		{
			TE_WARN("lua file was found in VFS, but not in Text! path: \"{}\"", relativePath);
		}
	}

	++_versionID;
	_iteratorEntries = std::nullopt;
}

void ScriptProvider::Deinitialize() noexcept
{
	//
}

ScriptID ScriptProvider::AddScriptImpl(std::string_view scriptName, TextID scriptTextID, std::string_view modUID)
{
	{
		auto it = _scriptName2ID.find(scriptName);
		if (it != _scriptName2ID.end())
		{
			TE_WARN("Attempt to alloc script id for existed script name: {}", scriptName);
			return it->second;
		}
	}

	++_latestScriptID;
	ScriptID scriptID = _latestScriptID;

	auto result = _scriptID2Entry.try_emplace(scriptID, std::string(scriptName), scriptTextID, modUID);
	if (!result.second) [[unlikely]]
	{
		throw std::runtime_error("Failed to add script");
	}

	const std::string &name = result.first->second.name;
	TE_TRACE("Add script <{}>{}", scriptID, name);
	_scriptName2ID.emplace(name, scriptID);
	++_versionID;
	_iteratorEntries = std::nullopt;

	return scriptID;
}

std::uint64_t ScriptProvider::GetVersionID() const noexcept
{
	return _versionID;
}

size_t ScriptProvider::GetCount() const noexcept
{
	return _scriptName2ID.size();
}

bool ScriptProvider::IsValidScript(ScriptID scriptID) const noexcept
{
	return _scriptID2Entry.contains(scriptID);
}

bool ScriptProvider::IsStaticScript(std::string_view scriptName) const noexcept
{
	return scriptName.starts_with(scriptPrefix);
}

ScriptID ScriptProvider::GetScriptIDByName(std::string_view scriptName) const noexcept
{
	std::string name{scriptName};
	auto it = _scriptName2ID.find(name);
	if (it != _scriptName2ID.end())
	{
		return it->second;
	}

	name = scriptNamespacePrefix + std::string(scriptName);
	it = _scriptName2ID.find(name);
	if (it != _scriptName2ID.end())
	{
		return it->second;
	}

	return false;
}

std::optional<std::string_view> ScriptProvider::GetScriptNameByID(ScriptID scriptID) const noexcept
{
	auto it = _scriptID2Entry.find(scriptID);
	if (it != _scriptID2Entry.end())
	{
		return it->second.name;
	}
	return std::nullopt;
}

ScriptID ScriptProvider::AddScript(std::string_view scriptName, TextID scriptTextID, std::string_view scriptModUID) noexcept
{
	if (scriptName.starts_with(scriptNamespace))
	{
		TE_INFO("{}", "Attempt to add static script");
		return false;
	}

	if (_scriptName2ID.contains(scriptName))
	{
		TE_INFO("Script has already been added: {}", scriptName);
	}

	return AddScriptImpl(scriptName, scriptTextID, scriptModUID);
}

bool ScriptProvider::RemoveScript(ScriptID scriptID) noexcept
{
	if (IScriptProvider::IsStaticScript(scriptID))
	{
		return false;
	}
	return RemoveScriptImpl(scriptID);
}

bool ScriptProvider::RemoveScriptImpl(ScriptID scriptID) noexcept
{
	auto it = _scriptID2Entry.find(scriptID);
	if (it == _scriptID2Entry.end())
	{
		return false;
	}

	TE_TRACE("Remove script <{}>{}", scriptID, it->second.name);
	_scriptName2ID.erase(it->second.name);
	_scriptID2Entry.erase(it);
	++_versionID;
	_iteratorEntries = std::nullopt;

	return true;
}

std::size_t ScriptProvider::RemoveScriptsBy(std::string_view uid) noexcept
{
	return std::erase_if(_scriptID2Entry, [this, &uid](const std::pair<const ScriptID, Entry> &pair)
	{
		if (pair.second.modUID != uid)
		{
			return false;
		}

		_scriptName2ID.erase(pair.second.name);
		++_versionID;
		_iteratorEntries = std::nullopt;

		return true;
	});
}

TextID ScriptProvider::GetScriptTextID(ScriptID scriptID) const noexcept
{
	auto it = _scriptID2Entry.find(scriptID);
	if (it == _scriptID2Entry.end())
	{
		return 0;
	}
	return it->second.textID;
}

std::shared_ptr<Text> ScriptProvider::GetScriptCode(ScriptID scriptID) const noexcept
{
	auto it = _scriptID2Entry.find(scriptID);
	if (it == _scriptID2Entry.end())
	{
		return nullptr;
	}

	TextID textID = it->second.textID;
	if (textID == 0)
	{
		return nullptr;
	}

	return GetGlobalResourcesCollection().GetTextResources().GetResource(textID);
}

std::string_view ScriptProvider::GetScriptModUID(ScriptID scriptID) noexcept
{
	auto it = _scriptID2Entry.find(scriptID);
	if (it == _scriptID2Entry.end())
	{
		return emptyString;
	}
	return it->second.modUID;
}

std::size_t ScriptProvider::GetEntriesSize() const
{
	return _scriptID2Entry.size();
}

std::vector<ScriptProvider::IteratorEntry>::const_iterator ScriptProvider::BeginEntries() const
{
	UpdateScriptEntriesCache();
	return _iteratorEntries->begin();
}

std::vector<ScriptProvider::IteratorEntry>::const_iterator ScriptProvider::EndEntries() const
{
	UpdateScriptEntriesCache();
	return _iteratorEntries->end();
}

void ScriptProvider::UpdateScriptEntriesCache() const noexcept
{
	if (_iteratorEntries.has_value()) [[likely]]
	{
		return;
	}

	_iteratorEntries = std::vector<IteratorEntry>();
	_iteratorEntries->reserve(_scriptID2Entry.size());

	for (const auto &[scriptID, entry] : _scriptID2Entry)
	{
		_iteratorEntries->push_back({
		    scriptID,
		    entry.name,
		    entry.textID,
		    entry.modUID,
		});
	}

	std::sort(_iteratorEntries->begin(), _iteratorEntries->end(), [](const IteratorEntry &l, const IteratorEntry &r) -> bool
	{
		return l.name < r.name;
	});
}

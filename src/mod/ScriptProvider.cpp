#include "ScriptProvider.h"

#include "util/Defs.h"
#include "util/StringUtils.hpp"

#include <filesystem>

using namespace tudov;

using ScriptID = ScriptID;

String StaticScriptNamespace(const String &path)
{
	return "#" + FilePathToLuaScriptName(path);
}

static constexpr const char *staticScriptPrefix = "#";
static constexpr const char *staticScriptDirectory = "lua";
static constexpr const char *staticScriptNamespace = "#lua";
static constexpr const char *staticScriptNamespacePrefix = "#lua.";

bool ScriptProvider::IsStaticScript(StringView scriptName)
{
	return scriptName.starts_with(staticScriptPrefix);
}

ScriptProvider::ScriptProvider(ModManager &modManager)
    : modManager(modManager),
      _log(Log::Get("ScriptProvider")),
      _latestScriptID(0)
{
	for (const auto &entry : std::filesystem::recursive_directory_iterator("lua"))
	{
		auto &&path = entry.path().string();
		auto &&scriptName = StaticScriptNamespace(path);

		AllocScript(scriptName, ReadFileToString(path));
	}
}

ScriptID ScriptProvider::AllocScript(StringView scriptName, StringView scriptCode, StringView namespace_)
{
	{
		auto &&it = _scriptName2ID.find(scriptName);
		if (it != _scriptName2ID.end())
		{
			_log->Warn("Attempt to alloc script id for existed script name: {}", scriptName);
			return it->second;
		}
	}

	++_latestScriptID;
	auto &&id = _latestScriptID;
	auto &&name = _scriptID2Entry.emplace(id, Entry(String(scriptName), String(scriptCode), namespace_)).first->second.name;
	_scriptName2ID.emplace(name, id);
	return id;
}

void ScriptProvider::DeallocScript(ScriptID scriptID)
{
	auto &&it = _scriptID2Entry.find(scriptID);
	if (it == _scriptID2Entry.end())
	{
		_log->Warn("Attempt to remove non-exist scriptID: {}", scriptID);
		return;
	}
	_scriptName2ID.erase(it->second.name);
	_scriptID2Entry.erase(it);
}

size_t ScriptProvider::GetCount() const noexcept
{
	return _scriptName2ID.size();
}

bool ScriptProvider::IsValidScriptID(ScriptID scriptID) const noexcept
{
	return _scriptID2Entry.contains(scriptID);
}

bool ScriptProvider::IsStaticScript(ScriptID scriptID) const noexcept
{
	auto &&it = _scriptID2Entry.find(scriptID);
	return it != _scriptID2Entry.end() && it->second.name.starts_with(staticScriptNamespace);
}

ScriptID ScriptProvider::GetScriptIDByName(StringView scriptName) const noexcept
{
	String name{scriptName};
	auto &&it = _scriptName2ID.find(name);
	if (it != _scriptName2ID.end())
	{
		return it->second;
	}

	name = staticScriptNamespacePrefix + String(scriptName);
	it = _scriptName2ID.find(name);
	if (it != _scriptName2ID.end())
	{
		return it->second;
	}

	return false;
}

Optional<StringView> ScriptProvider::GetScriptNameByID(ScriptID scriptID) const noexcept
{
	auto &&it = _scriptID2Entry.find(scriptID);
	if (it != _scriptID2Entry.end())
	{
		return it->second.name;
	}
	return nullopt;
}

ScriptID ScriptProvider::AddScript(StringView scriptName, StringView scriptCode, StringView namespace_) noexcept
{
	if (scriptName.starts_with(staticScriptNamespace))
	{
		_log->Info("Attempt to add static script");
		return false;
	}

	if (_scriptName2ID.contains(scriptName))
	{
		_log->Info("Script has already been added: {}", scriptName);
	}

	return AllocScript(scriptName, scriptCode, namespace_);
}

void ScriptProvider::RemoveScript(ScriptID scriptID) noexcept
{
	if (IsStaticScript(scriptID))
	{
		_log->Warn("Attempt to remove static script");
		return;
	}
	DeallocScript(scriptID);
}

const String &ScriptProvider::GetScriptCode(ScriptID scriptID) const noexcept
{
	auto &&it = _scriptID2Entry.find(scriptID);
	if (it == _scriptID2Entry.end())
	{
		return emptyString;
	}
	return it->second.code;
}

StringView ScriptProvider::GetScriptNamespace(ScriptID scriptID) noexcept
{
	auto &&it = _scriptID2Entry.find(scriptID);
	if (it == _scriptID2Entry.end())
	{
		return emptyString;
	}
	return it->second.namespace_;
}

// bool ScriptProvider::ContainsScript(StringView scriptName) const
// {
// 	return _scriptCodes.contains(scriptName);
// }

// Optional<String> ScriptProvider::TryResolveScriptName(StringView scriptName)
// {
// 	auto &&it = _scriptCodes.find(scriptName);
// 	if (it != _scriptCodes.end())
// 	{
// 		return it->first;
// 	}

// 	auto &&resolvedScriptName = staticScriptNamespacePrefix + String(scriptName);
// 	it = _scriptCodes.find(resolvedScriptName);
// 	if (it != _scriptCodes.end())
// 	{
// 		return resolvedScriptName;
// 	}

// 	return nullopt;
// }

UnorderedMap<ScriptID, ScriptProvider::Entry>::const_iterator ScriptProvider::begin() const
{
	return _scriptID2Entry.begin();
}

UnorderedMap<ScriptID, ScriptProvider::Entry>::const_iterator ScriptProvider::end() const
{
	return _scriptID2Entry.end();
}

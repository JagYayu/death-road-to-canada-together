#include "ScriptProvider.h"

#include "ScriptLoader.h"
#include "util/Defs.h"
#include "util/StringUtils.hpp"

#include <filesystem>

using namespace tudov;

using ScriptID = ScriptProvider::ScriptID;

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
      _currentScriptID(0)
{
	for (const auto &entry : std::filesystem::recursive_directory_iterator("lua"))
	{
		auto &&path = entry.path().string();
		auto &&scriptName = StaticScriptNamespace(path);

		auto &&[id, name] = AllocScript(scriptName);
		_scriptMap.emplace(id, Entry(StringView(name), ReadFileToString(path)));
	}
}

Tuple<ScriptID, StringView> ScriptProvider::AllocScript(StringView scriptName)
{
	++_currentScriptID;
	auto &&id = _currentScriptID;
	return {id, _scriptIDs.emplace(scriptName, id).first->first};
}

size_t ScriptProvider::GetCount() const noexcept
{
	return _scriptIDs.size();
}

bool ScriptProvider::IsValidScriptID(ScriptID scriptID) const noexcept
{
	return _scriptMap.contains(scriptID);
}

ScriptID ScriptProvider::GetScriptID(StringView scriptName) const noexcept
{
	String name{scriptName};
	auto &&it = _scriptIDs.find(name);
	if (it != _scriptIDs.end())
	{
		return it->second;
	}

	name = staticScriptNamespacePrefix + String(scriptName);
	it = _scriptIDs.find(name);
	if (it != _scriptIDs.end())
	{
		return it->second;
	}

	return invalidScriptID;
}

Optional<StringView> ScriptProvider::GetScriptName(ScriptID scriptID) const noexcept
{
	auto &&it = _scriptMap.find(scriptID);
	if (it != _scriptMap.end())
	{
		return it->second.name;
	}
	return null;
}

ScriptID ScriptProvider::AddScript(StringView scriptName, StringView scriptCode) noexcept
{
	if (scriptName.starts_with(staticScriptNamespace))
	{
		_log->Info("Attempt to add static script");
		return invalidScriptID;
	}

	if (_scriptIDs.contains(String(scriptName)))
	{
		_log->Info(Format("Script has already been added: {}", scriptName));
	}

	auto &&[id, name] = AllocScript(scriptName);
	_scriptMap.emplace(id, Entry(name, String(scriptCode)));
	return id;
}

void ScriptProvider::RemoveScript(ScriptID scriptID) noexcept
{
	auto &&it = _scriptMap.find(scriptID);
	if (!_scriptMap.contains(it->first))
	{
		_log->Info(Format("Attempt to remove non-exist script: {}", scriptID));
		return;
	}

	if (it->second.name.starts_with(staticScriptNamespace))
	{
		_log->Info("Attempt to remove static script");
		return;
	}

	_scriptIDs.erase(String(it->second.name));
	_scriptMap.erase(it);
}

const String &ScriptProvider::GetScriptCode(ScriptID scriptID) const noexcept
{
	auto &&it = _scriptMap.find(scriptID);
	if (it == _scriptMap.end())
	{
		return emptyString;
	}
	return it->second.code;
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

// 	return null;
// }

UnorderedMap<ScriptID, ScriptProvider::Entry>::const_iterator ScriptProvider::begin() const
{
	return _scriptMap.begin();
}

UnorderedMap<ScriptID, ScriptProvider::Entry>::const_iterator ScriptProvider::end() const
{
	return _scriptMap.end();
}

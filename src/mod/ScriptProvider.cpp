#include "ScriptProvider.h"

#include "util/StringUtils.hpp"

#include <filesystem>

using namespace tudov;

String ToStaticNamespace(const String &path)
{
	return "#" + FilePathToLuaScriptName(path);
}

ScriptProvider::ScriptProvider(ModManager &modManager)
    : modManager(modManager),
      _log(Log::Get("ScriptProvider"))
{
	auto &&directory = "lua";
	staticNamespace = ToStaticNamespace(directory);

	for (const auto &entry : std::filesystem::recursive_directory_iterator("lua"))
	{
		auto &&path = entry.path().string();
		_scriptMap[ToStaticNamespace(path)] = ReadFileToString(path);
	}
}

size_t ScriptProvider::GetCount() const
{
	return _scriptMap.size();
}

bool ScriptProvider::ContainsScript(const StringView &scriptName) const
{
	return _scriptMap.contains(scriptName);
}

const String &ScriptProvider::GetScript(const String &scriptName) const
{
	auto &&it = _scriptMap.find(scriptName);
	if (it == _scriptMap.end())
	{
		return emptyString;
	}
	return it->second;
}

Optional<String> ScriptProvider::TryRequireScript(const StringView &scriptName)
{
	auto &&it = _scriptMap.find(scriptName);
	if (it != _scriptMap.end())
	{
		return it->second;
	}

	it = _scriptMap.find(staticNamespace + String(scriptName));
	if (it != _scriptMap.end())
	{
		return it->second;
	}

	return emptyString;
}

void ScriptProvider::AddScript(const StringView &scriptName, const StringView &data)
{
	if (GetLuaNamespace(scriptName) == staticNamespace)
	{
		_log->Info("Attempt to add static script");
		return;
	}
	if (_scriptMap.contains(scriptName))
	{
		_log->Info(Format("Duplicated adding script: {}", StringView(scriptName)));
	}
	_scriptMap[String(scriptName)] = data;
}

void ScriptProvider::RemoveScript(const StringView &scriptName)
{
	if (!_scriptMap.contains(scriptName))
	{
		_log->Info(Format("Attempt to remove non-exist script: {}", scriptName));
		return;
	}
	if (GetLuaNamespace(scriptName) == staticNamespace)
	{
		_log->Info("Attempt to remove static script");
		return;
	}
	_scriptMap.erase(String(scriptName));
}

UnorderedMap<String, String>::const_iterator ScriptProvider::begin() const
{
	return _scriptMap.begin();
}

UnorderedMap<String, String>::const_iterator ScriptProvider::end() const
{
	return _scriptMap.end();
}

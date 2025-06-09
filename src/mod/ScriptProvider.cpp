#include "ScriptProvider.h"

#include "util/StringUtils.hpp"

#include <filesystem>
#include <format>
#include <string>
#include <string_view>

using namespace tudov;

ScriptProvider::ScriptProvider(ModManager &modManager)
    : _modManager(modManager),
      _log("ScriptProvider")
{
	for (const auto &entry : std::filesystem::recursive_directory_iterator("lua"))
	{
		auto &&path = entry.path().string();
		_scriptMap["#" + FilePathToLuaScriptName(path)] = ReadFileToString(path);
	}
}

size_t ScriptProvider::GetCount() const
{
	return _scriptMap.size();
}

bool ScriptProvider::ContainsScript(const std::string_view &name) const
{
	return _scriptMap.contains(name);
}

const std::string &ScriptProvider::GetScript(const std::string &name) const
{
	auto &&it = _scriptMap.find(name);
	if (it == _scriptMap.end())
	{
		return emptyString;
	}
	return it->second;
}

void ScriptProvider::AddScript(const std::string_view &name, const std::string_view &data)
{
	if (_scriptMap.contains(name))
	{
		_log.Info(std::format("Duplicated adding script: {}", std::string_view(name)));
	}
	_scriptMap[std::string(name)] = data;
}

void ScriptProvider::RemoveScript(const std::string_view &name)
{
	if (!_scriptMap.contains(name))
	{
		_log.Info(std::format("Attempt to remove non-exist script: {}", std::string_view(name)));
	}
	_scriptMap.erase(std::string(name));
}

std::unordered_map<std::string, std::string>::const_iterator ScriptProvider::begin() const
{
	return _scriptMap.begin();
}

std::unordered_map<std::string, std::string>::const_iterator ScriptProvider::end() const
{
	return _scriptMap.end();
}

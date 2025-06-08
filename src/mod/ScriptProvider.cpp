#include "ScriptProvider.h"
#include <format>

using namespace tudov;

ScriptProvider::ScriptProvider(ModManager &modManager)
    : _modManager(modManager),
      _log("ScriptProvider")
{
}

size_t ScriptProvider::GetCount() const
{
	return _scriptMap.size();
}

void ScriptProvider::AddScript(const std::string &name, const std::string &data)
{
	if (_scriptMap.contains(name))
	{
		_log.Info(std::format("Duplicated adding script: {}", std::string_view(name)));
	}
	_scriptMap[name] = data;
}

void ScriptProvider::RemoveScript(const std::string &name)
{
	if (!_scriptMap.contains(name))
	{
		_log.Info(std::format("Attempt to remove non-exist script: {}", std::string_view(name)));
	}
	_scriptMap.erase(name);
}

std::map<std::string, std::string>::const_iterator ScriptProvider::begin() const
{
	return _scriptMap.begin();
}

std::map<std::string, std::string>::const_iterator ScriptProvider::end() const
{
	return _scriptMap.end();
}

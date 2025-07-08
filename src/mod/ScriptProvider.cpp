#include "ScriptProvider.h"

#include "util/Defs.h"
#include "util/StringUtils.hpp"

#include <filesystem>

using namespace tudov;

static constexpr const char *scriptPrefix = "#";
static constexpr const char *scriptDirectory = "lua";
static constexpr const char *scriptNamespace = "#lua";
static constexpr const char *scriptNamespacePrefix = "#lua.";

std::string StaticScriptNamespace(const std::string &path)
{
	return scriptPrefix + FilePathToLuaScriptName(path);
}

ScriptProvider::ScriptProvider(Context &context)
    : _context(context),
      _log(Log::Get("ScriptProvider")),
      _latestScriptID(0)
{
	for (const auto &entry : std::filesystem::recursive_directory_iterator("lua"))
	{
		auto &&path = entry.path().string();
		auto &&scriptName = StaticScriptNamespace(path);

		AddScriptImpl(scriptName, ReadFileToString(path));
	}
}

Context &ScriptProvider::GetContext() noexcept
{
	return _context;
}

ScriptID ScriptProvider::AddScriptImpl(std::string_view scriptName, std::string_view scriptCode, std::string_view namespace_)
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
	auto &&name = _scriptID2Entry.emplace(id, Entry(std::string(scriptName), std::string(scriptCode), namespace_)).first->second.name;
	_scriptName2ID.emplace(name, id);
	return id;
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
	auto &&it = _scriptName2ID.find(name);
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
	auto &&it = _scriptID2Entry.find(scriptID);
	if (it != _scriptID2Entry.end())
	{
		return it->second.name;
	}
	return std::nullopt;
}

ScriptID ScriptProvider::AddScript(std::string_view scriptName, std::string_view scriptCode, std::string_view namespace_) noexcept
{
	if (scriptName.starts_with(scriptNamespace))
	{
		_log->Info("Attempt to add static script");
		return false;
	}

	if (_scriptName2ID.contains(scriptName))
	{
		_log->Info("Script has already been added: {}", scriptName);
	}

	return AddScriptImpl(scriptName, scriptCode, namespace_);
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
	auto &&it = _scriptID2Entry.find(scriptID);
	if (it == _scriptID2Entry.end())
	{
		// _log->Warn("Attempt to remove non-exist scriptID: {}", scriptID);
		return false;
	}
	_scriptName2ID.erase(it->second.name);
	_scriptID2Entry.erase(it);
	return true;
}

std::size_t ScriptProvider::RemoveScriptBy(std::string_view namespace_) noexcept
{
	return std::erase_if(_scriptID2Entry, [&](const std::pair<const ScriptID, Entry> &pair)
	{
		if (pair.second.namespace_ == namespace_)
		{
			_scriptName2ID.erase(pair.second.name);
			return true;
		}
		return false;
	});
}

const std::string &ScriptProvider::GetScriptCode(ScriptID scriptID) const noexcept
{
	auto &&it = _scriptID2Entry.find(scriptID);
	if (it == _scriptID2Entry.end())
	{
		return emptyString;
	}
	return it->second.code;
}

std::string_view ScriptProvider::GetScriptNamespace(ScriptID scriptID) noexcept
{
	auto &&it = _scriptID2Entry.find(scriptID);
	if (it == _scriptID2Entry.end())
	{
		return emptyString;
	}
	return it->second.namespace_;
}

ScriptProvider::TScriptID2Entry::const_iterator ScriptProvider::begin() const
{
	return _scriptID2Entry.begin();
}

ScriptProvider::TScriptID2Entry::const_iterator ScriptProvider::end() const
{
	return _scriptID2Entry.end();
}

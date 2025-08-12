#include "mod/ScriptProvider.hpp"

#include "data/VirtualFileSystem.hpp"
#include "resource/GlobalResourcesCollection.hpp"
#include "util/Definitions.hpp"
#include "util/StringUtils.hpp"
#include <cassert>
#include <string_view>
#include <vector>

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
	std::vector<std::string_view> luaFilePaths = GetVirtualFileSystem().List(directory, VirtualFileSystem::ListOption::File);
	auto textResources = GetGlobalResourcesCollection().GetTextResources();

	for (auto &&relativePath : luaFilePaths)
	{
		auto &&fullPath = (std::filesystem::path(directory) / relativePath).generic_string();
		ResourceID id = textResources.GetResourceID(fullPath);

		if (id != 0) [[likely]]
		{
			auto &&scriptName = StaticScriptNamespace(std::string(relativePath));
			auto code = textResources.GetResource(id);
			assert(code != nullptr);

			AddScriptImpl(scriptName, code);
		}
		else [[unlikely]]
		{
			Warn("lua file was found in VFS, but not in TextResource! path: \"{}\"", relativePath);
		}
	}
}

void ScriptProvider::Deinitialize() noexcept
{
	//
}

ScriptID ScriptProvider::AddScriptImpl(std::string_view scriptName, const std::shared_ptr<TextResource> &scriptCode, std::string_view modUID)
{
	{
		auto &&it = _scriptName2ID.find(scriptName);
		if (it != _scriptName2ID.end())
		{
			Warn("Attempt to alloc script id for existed script name: {}", scriptName);
			return it->second;
		}
	}

	++_latestScriptID;
	auto &&id = _latestScriptID;
	auto &&name = _scriptID2Entry.try_emplace(id, std::string(scriptName), scriptCode, modUID).first->second.name;
	Trace("Add script <{}>\"{}\"", id, name);
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

ScriptID ScriptProvider::AddScript(std::string_view scriptName, const std::shared_ptr<TextResource> &scriptCode, std::string_view scriptModUID) noexcept
{
	if (scriptName.starts_with(scriptNamespace))
	{
		Info("Attempt to add static script");
		return false;
	}

	if (_scriptName2ID.contains(scriptName))
	{
		Info("Script has already been added: {}", scriptName);
	}

	return AddScriptImpl(scriptName, scriptCode, scriptModUID);
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
		return false;
	}
	Trace("Remove script <{}>\"{}\"", scriptID, it->second.name);
	_scriptName2ID.erase(it->second.name);
	_scriptID2Entry.erase(it);
	return true;
}

std::size_t ScriptProvider::RemoveScriptsBy(std::string_view uid) noexcept
{
	return std::erase_if(_scriptID2Entry, [this, &uid](const std::pair<const ScriptID, Entry> &pair)
	{
		if (pair.second.modUID == uid)
		{
			_scriptName2ID.erase(pair.second.name);
			return true;
		}
		return false;
	});
}

std::shared_ptr<TextResource> ScriptProvider::GetScriptCode(ScriptID scriptID) const noexcept
{
	auto &&it = _scriptID2Entry.find(scriptID);
	if (it == _scriptID2Entry.end())
	{
		return nullptr;
	}
	return it->second.code;
}

std::string_view ScriptProvider::GetScriptModUID(ScriptID scriptID) noexcept
{
	auto &&it = _scriptID2Entry.find(scriptID);
	if (it == _scriptID2Entry.end())
	{
		return emptyString;
	}
	return it->second.modUID;
}

ScriptProvider::TScriptID2Entry::const_iterator ScriptProvider::begin() const
{
	return _scriptID2Entry.begin();
}

ScriptProvider::TScriptID2Entry::const_iterator ScriptProvider::end() const
{
	return _scriptID2Entry.end();
}

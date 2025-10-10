/**
 * @file mod/ScriptErrors.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "mod/ScriptErrors.hpp"

#include "mod/ScriptLoader.hpp"
#include "sol/string_view.hpp"
#include "util/Definitions.hpp"

#include <algorithm>

#include <cmath>
#include <memory>
#include <optional>
#include <regex>

using namespace tudov;

std::uint32_t ScriptError::ExtractLuaErrorLine(std::string_view error)
{
	std::regex pattern{R"((?:\[string "[^"]*"\]|.+?):(\d+):)"};
	const char *begin = error.data();
	const char *end = begin + error.size();

	std::cregex_iterator it(begin, end, pattern);
	std::cregex_iterator endIt;

	if (it != endIt)
	{
		const std::cmatch &match = *it;
		if (match.size() >= 2)
		{
			try
			{
				return static_cast<std::uint32_t>(std::stoi(match[1].str()));
			}
			catch (...)
			{
				return 0;
			}
		}
	}

	return 0;
}

ScriptError::ScriptError(ScriptID scriptID, std::string_view message) noexcept
    : time(std::chrono::system_clock::now()),
      scriptID(scriptID),
      line(ScriptError::ExtractLuaErrorLine(message)),
      message(std::string(message))
{
}

ScriptErrors::ScriptErrors(Context &context) noexcept
    : _context(context)
{
}

Context &ScriptErrors::GetContext() noexcept
{
	return _context;
}

void ScriptErrors::PreInitialize() noexcept
{
	IScriptLoader &scriptLoader = GetScriptLoader();

	_handlerIDOnPreLoadAllScripts = scriptLoader.GetOnPreLoadAllScripts() += [this]() -> void
	{
		ClearCaches();
	};

	_handlerIDOnUnloadScript = scriptLoader.GetOnUnloadScript() += [this](ScriptID scriptID, std::string_view scriptName) -> void
	{
		RemoveLoadtimeError(scriptID);
	};

	_handlerIDOnFailedLoadScript = scriptLoader.GetOnFailedLoadScript() += [this](ScriptID scriptID, std::string_view scriptName, std::string_view error) -> void
	{
		AddLoadtimeError(std::make_shared<ScriptError>(scriptID, error));
	};
}

void ScriptErrors::PostDeinitialize() noexcept
{
	IScriptLoader &scriptLoader = GetScriptLoader();

	scriptLoader.GetOnPreLoadAllScripts() -= _handlerIDOnPreLoadAllScripts;
	scriptLoader.GetOnUnloadScript() -= _handlerIDOnUnloadScript;
	scriptLoader.GetOnFailedLoadScript() -= _handlerIDOnFailedLoadScript;

	_handlerIDOnPreLoadAllScripts = 0;
	_handlerIDOnUnloadScript = 0;
	_handlerIDOnFailedLoadScript = 0;
}

bool ScriptErrors::HasLoadtimeError() const noexcept
{
	return !_scriptLoadtimeErrors.empty();
}

bool ScriptErrors::HasRuntimeError() const noexcept
{
	return !GetRuntimeErrorsCached().empty();
}

std::vector<std::shared_ptr<ScriptError>> ScriptErrors::GetLoadtimeErrors() const noexcept
{
	std::vector<std::shared_ptr<ScriptError>> errors{};
	errors.reserve(_scriptLoadtimeErrors.size());

	for (auto &[scriptID, error] : _scriptLoadtimeErrors)
	{
		TE_ASSERT(error != nullptr);
		errors.emplace_back(error);
	}

	std::sort(errors.begin(), errors.end(), [](std::shared_ptr<ScriptError> &l, std::shared_ptr<ScriptError> &r) -> bool
	{
		return l->time < r->time;
	});

	return errors;
}

std::vector<std::shared_ptr<ScriptError>> ScriptErrors::GetRuntimeErrors() const noexcept
{
	return GetRuntimeErrorsCached();
}

const std::vector<std::shared_ptr<ScriptError>> &ScriptErrors::GetLoadtimeErrorsCached() const noexcept
{
	if (_scriptLoadtimeErrorCache == std::nullopt) [[unlikely]]
	{
		_scriptLoadtimeErrorCache = GetLoadtimeErrors();
	}

	return *_scriptLoadtimeErrorCache;
}

const std::vector<std::shared_ptr<ScriptError>> &ScriptErrors::GetRuntimeErrorsCached() const noexcept
{
	if (_scriptRuntimeErrorsCached == std::nullopt) [[unlikely]]
	{
		_scriptRuntimeErrorsCached = std::make_optional<std::vector<std::shared_ptr<ScriptError>>>();

		const IScriptProvider &scriptProvider = GetScriptProvider();
		for (const auto &scriptError : _scriptRuntimeErrors)
		{
			if (scriptProvider.IsValidScript(scriptError->scriptID))
			{
				_scriptRuntimeErrorsCached->emplace_back(scriptError);
			}
		}

		std::sort(_scriptRuntimeErrorsCached->begin(), _scriptRuntimeErrorsCached->end(), [](const std::shared_ptr<ScriptError> &l, const std::shared_ptr<ScriptError> &r) -> bool
		{
			return l->time > r->time;
		});
	}

	return *_scriptRuntimeErrorsCached;
}

void ScriptErrors::ClearCaches() noexcept
{
	_scriptLoadtimeErrorCache = std::nullopt;
	_scriptRuntimeErrorsCached = std::nullopt;
}

void ScriptErrors::AddLoadtimeError(const std::shared_ptr<ScriptError> &scriptError)
{
	_scriptLoadtimeErrors[scriptError->scriptID] = scriptError;
	ClearCaches();
}

void ScriptErrors::AddRuntimeError(const std::shared_ptr<ScriptError> &scriptError)
{
	_scriptRuntimeErrors.push_back(scriptError);

	if (_scriptRuntimeErrors.size() > 1024) [[unlikely]]
	{
		for (auto i = 1; i < 256; ++i)
		{
			_scriptRuntimeErrors.pop_front();
		}
	}

	ClearCaches();
}

bool ScriptErrors::RemoveLoadtimeError(ScriptID scriptID)
{
	if (_scriptLoadtimeErrors.erase(scriptID))
	{
		ClearCaches();
		return true;
	}
	return false;
}

bool ScriptErrors::RemoveRuntimeError(ScriptID scriptID)
{
	std::size_t count = std::erase_if(_scriptRuntimeErrors, [scriptID](const std::shared_ptr<ScriptError> &scriptError) -> bool
	{
		return scriptError->scriptID == scriptID;
	});

	if (count != 0)
	{
		ClearCaches();
		return true;
	}
	return false;
}

void ScriptErrors::ClearLoadtimeErrors() noexcept
{
	_scriptLoadtimeErrors.clear();
	ClearCaches();
}

void ScriptErrors::ClearRuntimeErrors() noexcept
{
	_scriptRuntimeErrors.clear();
	ClearCaches();
}

void ScriptErrors::LuaAddLoadtimeError(sol::object scriptID, sol::object traceback) noexcept
{
	auto scriptID_ = static_cast<ScriptID>(scriptID.as<std::double_t>());
	std::string_view traceback_ = traceback.as<sol::string_view>();

	IScriptErrors::AddLoadtimeError(scriptID_, traceback_);
}

void ScriptErrors::LuaAddRuntimeError(sol::object scriptID, sol::object traceback) noexcept
{
	auto scriptID_ = static_cast<ScriptID>(scriptID.as<std::double_t>());
	std::string_view traceback_ = traceback.as<sol::string_view>();

	IScriptErrors::AddRuntimeError(scriptID_, traceback_);
}

bool ScriptErrors::LuaRemoveLoadtimeError(sol::object scriptID) noexcept
{
	return RemoveLoadtimeError(scriptID.as<ScriptID>());
}

bool ScriptErrors::LuaRemoveRuntimeError(sol::object scriptID) noexcept
{
	return RemoveRuntimeError(scriptID.as<ScriptID>());
}

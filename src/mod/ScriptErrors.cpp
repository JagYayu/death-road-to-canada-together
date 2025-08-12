#include "mod/ScriptErrors.hpp"

#include <cassert>
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

ScriptError::ScriptError(ScriptID scriptID, EScriptErrorType type, std::string_view message) noexcept
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

bool ScriptErrors::HasLoadtimeError() const noexcept
{
	return !_scriptLoadtimeErrors.empty();
}

bool ScriptErrors::HasRuntimeError() const noexcept
{
	return !_scriptRuntimeErrors.empty();
}

std::vector<std::shared_ptr<ScriptError>> ScriptErrors::GetLoadtimeErrors() const noexcept
{
	std::vector<std::shared_ptr<ScriptError>> errors{};
	errors.reserve(_scriptLoadtimeErrors.size());

	for (auto &[scriptID, error] : _scriptLoadtimeErrors)
	{
		assert(error != nullptr);
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
	return _scriptRuntimeErrors;
}

const std::vector<std::shared_ptr<ScriptError>> &ScriptErrors::GetLoadtimeErrorsCached() noexcept
{
	if (_scriptLoadtimeErrorCached == std::nullopt) [[unlikely]]
	{
		_scriptLoadtimeErrorCached = GetLoadtimeErrors();
	}

	return *_scriptLoadtimeErrorCached;
}

const std::vector<std::shared_ptr<ScriptError>> &ScriptErrors::GetRuntimeErrorsCached() noexcept
{
	return _scriptRuntimeErrors;
}

void ScriptErrors::ClearCaches() noexcept
{
	_scriptLoadtimeErrorCached = std::nullopt;
}

void ScriptErrors::AddLoadtimeError(const std::shared_ptr<ScriptError> &scriptError)
{
	_scriptLoadtimeErrors[scriptError->scriptID] = scriptError;
	ClearCaches();
}

void ScriptErrors::AddRuntimeError(const std::shared_ptr<ScriptError> &scriptError)
{
	_scriptRuntimeErrors.emplace_back(scriptError);
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

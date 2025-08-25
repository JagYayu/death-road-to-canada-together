/**
 * @file exception/ScriptException.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "exception/ScriptException.hpp"

#include "event/EventManager.hpp"
#include "mod/ScriptProvider.hpp"

using namespace tudov;

ScriptException::ScriptException(Context &context, ScriptID scriptID, std::string traceback) noexcept
    : Exception(context),
      scriptID(scriptID),
      traceback(traceback)
{
}

std::string_view ScriptException::What() const noexcept
{
	std::string_view scriptName = GetScriptProvider().GetScriptNameByID(scriptID).value_or("$UNKNOWN$");
	_what = std::format("Script exception <{}>\"{}\"{}", scriptID, scriptName, traceback);
	return _what;
}

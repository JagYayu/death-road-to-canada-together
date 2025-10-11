/**
 * @file event/CoreEventsData.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Event/CoreEventsData.hpp"

#include "Debug/Debug.hpp"
#include "Debug/DebugConsole.hpp"
#include "Debug/DebugManager.hpp"
#include "Mod/ScriptEngine.hpp"
#include "Program/Context.hpp"

#include "sol/forward.hpp"
#include "sol/protected_function_result.hpp"

#include <memory>

using namespace tudov;

std::shared_ptr<CoreEventData> CoreEventData::Extract(sol::object e) noexcept
{
	if (e.is<sol::table>()) [[likely]]
	{
		auto data = e.as<sol::table>()["data"];
		if (data.valid() && data.is<std::shared_ptr<CoreEventData>>()) [[likely]]
		{
			return data.get<std::shared_ptr<CoreEventData>>();
		}
	}

	return nullptr;
}

EventDebugProvideData::EventDebugProvideData(Context &context, DebugManager &debugManager) noexcept
    : context(context),
      debugManager(debugManager)
{
}

void EventDebugProvideData::LuaAddElement(sol::table args) noexcept
{
	// debugManager->AddElement();
}

void EventDebugProvideData::LuaSetDebugCommand(sol::table args) noexcept
{
	DebugConsole *debugConsole = debugManager.::IDebugManager::GetElement<DebugConsole>();
	if (debugConsole == nullptr)
	{
		return;
	}

	auto &scriptEngine = context.GetScriptEngine();
	scriptEngine.DebugTraceback();

	auto name = args["name"];
	auto help = args["help"];
	auto func = args["func"];
	if (!name.valid() or !name.is<sol::string_view>()) [[unlikely]]
	{
		scriptEngine.ThrowError("Bad field 'name': string expected", 2);
	}
	else if (!help.valid() or !help.is<sol::string_view>()) [[unlikely]]
	{
		scriptEngine.ThrowError("Bad field 'help': string expected", 2);
	}
	else if (!func.valid() or !func.is<sol::protected_function>()) [[unlikely]]
	{
		scriptEngine.ThrowError("Bad field 'help': function expected", 2);
	}

	auto luaFunc = func.get<sol::protected_function>();
	auto &&cmdFunc = [luaFunc](std::string_view arg) -> std::vector<DebugConsoleResult>
	{
		std::vector<DebugConsoleResult> results;

		sol::protected_function_result result = luaFunc(arg);
		if (!result.valid())
		{
			sol::error err = result;
			results.emplace_back(std::string(err.what()), EDebugConsoleCode::Failure);
		}
		else
		{
			auto list = result.get<sol::table>();
			if (!list.valid())
			{
				results.emplace_back("Bad return value, table expected!", EDebugConsoleCode::Failure);
			}
			else
			{
				for (std::size_t i = 1;;)
				{
					if (!list[i].valid() || !list[i].is<sol::string_view>() || !list[i + 1].valid() || !list[i + 1].is<std::double_t>())
					{
						break;
					}

					results.emplace_back(std::string(list[i].get<sol::string_view>()), static_cast<EDebugConsoleCode>(list[i + 1].get<std::double_t>()));
					i += 2;
				}
			}
		}

		return results;
	};

	DebugConsole::Command command{
	    std::string(name.get<std::string_view>()),
	    std::string(help.get<std::string_view>()),
	    cmdFunc,
	};
	debugConsole->SetCommand(command);
}

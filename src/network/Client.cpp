/**
 * @file network/Client.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "network/Client.hpp"
#include "mod/ScriptEngine.hpp"
#include "util/Micros.hpp"
#include "util/Utils.hpp"

#include "sol/string_view.hpp"

using namespace tudov;

void IClient::LuaConnect(sol::object args)
{
	//
}

void IClient::LuaSendReliable(sol::object data)
{
	if (!data.is<sol::string_view>()) [[unlikely]]
	{
		throw std::runtime_error(std::format("bad argument to #1 '{}' (string expected, got %s)", TE_NAMEOF(data), GetLuaTypeStringView(data.get_type())));
	}

	SendReliable(data.as<sol::string_view>());
}

void IClient::LuaSendUnreliable(sol::object data)
{
	if (!data.is<sol::string_view>()) [[unlikely]]
	{
		throw std::runtime_error(std::format("bad argument to #1 '{}' (string expected, got %s)", TE_NAMEOF(data), GetLuaTypeStringView(data.get_type())));
	}

	SendUnreliable(data.as<sol::string_view>());
}

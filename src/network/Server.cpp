/**
 * @file network/Server.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "network/Server.hpp"

#include "mod/ScriptEngine.hpp"

using namespace tudov;

void IServer::LuaHost(sol::object args) noexcept
{
	TE_ASSERT(false, "Not implement yet");
}

void IServer::LuaSendReliable(sol::object clientID, sol::object data) noexcept
{
	if (!clientID.is<std::double_t>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("bad argument to #1 '{}' (number expected, got %s)", TE_NAMEOF(clientID), GetLuaTypeStringView(clientID.get_type()));
	}

	if (!data.is<sol::string_view>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("bad argument to #2 '{}' (string expected, got %s)", TE_NAMEOF(data), GetLuaTypeStringView(data.get_type()));
	}

	SendReliable(static_cast<std::int32_t>(clientID.as<std::double_t>()), data.as<sol::string_view>());
}

void IServer::LuaSendUnreliable(sol::object clientID, sol::object data) noexcept
{
	if (!clientID.is<std::double_t>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("bad argument to #1 '{}' (number expected, got %s)", TE_NAMEOF(clientID), GetLuaTypeStringView(clientID.get_type()));
	}

	if (!data.is<sol::string_view>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("bad argument to #2 '{}' (string expected, got %s)", TE_NAMEOF(data), GetLuaTypeStringView(data.get_type()));
	}

	SendUnreliable(static_cast<std::int32_t>(clientID.as<std::double_t>()), data.as<sol::string_view>());
}

void IServer::LuaBroadcastReliable(sol::object data) noexcept
{
	if (!data.is<sol::string_view>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("bad argument to #1 '{}' (string expected, got %s)", TE_NAMEOF(data), GetLuaTypeStringView(data.get_type()));
	}

	BroadcastReliable(data.as<sol::string_view>());
}

void IServer::LuaBroadcastUnreliable(sol::object data) noexcept
{
	if (!data.is<sol::string_view>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("bad argument to #1 '{}' (string expected, got %s)", TE_NAMEOF(data), GetLuaTypeStringView(data.get_type()));
	}

	BroadcastUnreliable(data.as<sol::string_view>());
}

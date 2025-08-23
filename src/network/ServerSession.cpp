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

#include "network/ServerSession.hpp"

#include "mod/ScriptEngine.hpp"
#include <span>

using namespace tudov;

void IServerSession::LuaHost(sol::object args) noexcept
{
	TE_ASSERT(false, "Not implement yet");
}

void IServerSession::LuaSendReliable(sol::object clientID, sol::object data, sol::object channelID) noexcept
{
	if (!clientID.is<std::double_t>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("bad argument to #1 '{}' (number expected, got %s)", TE_NAMEOF(clientID), GetLuaTypeStringView(clientID.get_type()));
	}

	if (!data.is<sol::string_view>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("bad argument to #2 '{}' (string expected, got %s)", TE_NAMEOF(data), GetLuaTypeStringView(data.get_type()));
	}

	auto str = data.as<sol::string_view>();
	std::span<const std::byte> data_{reinterpret_cast<const std::byte *>(str.data()), str.size()};

	SendReliable(clientID.as<std::double_t>(), data_, channelID.as<std::double_t>());
}

void IServerSession::LuaSendUnreliable(sol::object clientID, sol::object data, sol::object channelID) noexcept
{
	if (!clientID.is<std::double_t>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("bad argument to #1 '{}' (number expected, got %s)", TE_NAMEOF(clientID), GetLuaTypeStringView(clientID.get_type()));
	}

	if (!data.is<sol::string_view>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("bad argument to #2 '{}' (string expected, got %s)", TE_NAMEOF(data), GetLuaTypeStringView(data.get_type()));
	}

	auto str = data.as<sol::string_view>();
	std::span<const std::byte> data_{reinterpret_cast<const std::byte *>(str.data()), str.size()};

	SendUnreliable(static_cast<std::int32_t>(clientID.as<std::double_t>()), data_, channelID.as<std::double_t>());
}

void IServerSession::LuaBroadcastReliable(sol::object data, sol::object channelID) noexcept
{
	if (!data.is<sol::string_view>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("bad argument to #1 '{}' (string expected, got %s)", TE_NAMEOF(data), GetLuaTypeStringView(data.get_type()));
	}

	auto str = data.as<sol::string_view>();
	std::span<const std::byte> data_{reinterpret_cast<const std::byte *>(str.data()), str.size()};

	BroadcastReliable(data_, channelID.as<std::double_t>());
}

void IServerSession::LuaBroadcastUnreliable(sol::object data, sol::object channelID) noexcept
{
	if (!data.is<sol::string_view>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("bad argument to #1 '{}' (string expected, got %s)", TE_NAMEOF(data), GetLuaTypeStringView(data.get_type()));
	}

	auto str = data.as<sol::string_view>();
	std::span<const std::byte> data_{reinterpret_cast<const std::byte *>(str.data()), str.size()};

	BroadcastUnreliable(data_, channelID.as<std::double_t>());
}

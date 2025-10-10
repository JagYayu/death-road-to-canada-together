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
#include "network/NetworkSessionData.hpp"

#include <span>

using namespace tudov;

void IServerSession::LuaHost(sol::object args) noexcept
{
	TE_ASSERT(false, "Not implement yet");
}

void IServerSession::LuaDisconnect(sol::object clientID, sol::object code) noexcept
{
	if (!clientID.is<std::double_t>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("Bad argument to #1 '{}' (number expected, got %s)", TE_NAMEOF(clientID), GetLuaTypeStringView(clientID.get_type()));
	}

	if (!code.is<std::int32_t>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("Bad argument to #2 '{}' (number expected, got %s)", TE_NAMEOF(code), GetLuaTypeStringView(code.get_type()));
	}

	Disconnect(clientID.as<std::double_t>(), code.as<EDisconnectionCode>());
}

void IServerSession::LuaSendReliable(sol::object clientID, sol::object data, sol::object channelID) noexcept
{
	if (!clientID.is<std::double_t>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("Bad argument to #1 '{}' (number expected, got %s)", TE_NAMEOF(clientID), GetLuaTypeStringView(clientID.get_type()));
	}

	if (!data.is<sol::string_view>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("Bad argument to #2 '{}' (string expected, got %s)", TE_NAMEOF(data), GetLuaTypeStringView(data.get_type()));
	}

	auto str = data.as<sol::string_view>();

	NetworkSessionData data_{
	    .bytes = std::span<const std::byte>(reinterpret_cast<const std::byte *>(str.data()), str.size()),
	    .channelID = static_cast<ChannelID>(channelID.as<std::double_t>()),
	};
	SendReliable(clientID.as<std::double_t>(), data_);
}

void IServerSession::LuaSendUnreliable(sol::object clientID, sol::object data, sol::object channelID) noexcept
{
	if (!clientID.is<std::double_t>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("Bad argument to #1 '{}' (number expected, got %s)", TE_NAMEOF(clientID), GetLuaTypeStringView(clientID.get_type()));
	}

	if (!data.is<sol::string_view>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("Bad argument to #2 '{}' (string expected, got %s)", TE_NAMEOF(data), GetLuaTypeStringView(data.get_type()));
	}

	auto str = data.as<sol::string_view>();

	NetworkSessionData data_{
	    .bytes = std::span<const std::byte>(reinterpret_cast<const std::byte *>(str.data()), str.size()),
	    .channelID = static_cast<ChannelID>(channelID.as<std::double_t>()),
	};
	SendUnreliable(static_cast<std::int32_t>(clientID.as<std::double_t>()), data_);
}

void IServerSession::LuaBroadcastReliable(sol::object data, sol::object channelID) noexcept
{
	if (!data.is<sol::string_view>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("Bad argument to #1 '{}' (string expected, got %s)", TE_NAMEOF(data), GetLuaTypeStringView(data.get_type()));
	}

	auto str = data.as<sol::string_view>();

	NetworkSessionData data_{
	    .bytes = std::span<const std::byte>(reinterpret_cast<const std::byte *>(str.data()), str.size()),
	    .channelID = static_cast<ChannelID>(channelID.as<std::double_t>()),
	};
	BroadcastReliable(data_);
}

void IServerSession::LuaBroadcastUnreliable(sol::object data, sol::object channelID) noexcept
{
	if (!data.is<sol::string_view>()) [[unlikely]]
	{
		GetScriptEngine().ThrowError("Bad argument to #1 '{}' (string expected, got %s)", TE_NAMEOF(data), GetLuaTypeStringView(data.get_type()));
	}

	auto str = data.as<sol::string_view>();

	NetworkSessionData data_{
	    .bytes = std::span<const std::byte>(reinterpret_cast<const std::byte *>(str.data()), str.size()),
	    .channelID = static_cast<ChannelID>(channelID.as<std::double_t>()),
	};
	BroadcastUnreliable(data_);
}

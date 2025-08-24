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

#include "network/ClientSession.hpp"
#include "network/NetworkSessionData.hpp"

#include "util/Definitions.hpp"
#include "util/Micros.hpp"
#include "util/Utils.hpp"

#include "sol/string_view.hpp"
#include <cmath>
#include <vector>

using namespace tudov;

void IClientSession::LuaConnect(sol::object args)
{
	// TODO
}

void IClientSession::LuaSendReliable(sol::object data, sol::object channelID)
{
	if (!data.is<sol::string_view>()) [[unlikely]]
	{
		throw std::runtime_error(std::format("bad argument to #1 '{}' (string expected, got %s)", TE_NAMEOF(data), GetLuaTypeStringView(data.get_type())));
	}

	auto str = data.as<sol::string_view>();

	NetworkSessionData data_{
	    .bytes = std::span<const std::byte>(reinterpret_cast<const std::byte *>(str.data()), str.size() + 1),
	    .channelID = static_cast<ChannelID>(channelID.as<std::double_t>()),
	};
	SendReliable(data_);
}

void IClientSession::LuaSendUnreliable(sol::object data, sol::object channelID)
{
	if (!data.is<sol::string_view>()) [[unlikely]]
	{
		throw std::runtime_error(std::format("bad argument to #1 '{}' (string expected, got %s)", TE_NAMEOF(data), GetLuaTypeStringView(data.get_type())));
	}

	auto str = data.as<sol::string_view>();

	NetworkSessionData data_{
	    .bytes = std::span<const std::byte>(reinterpret_cast<const std::byte *>(str.data()), str.size() + 1),
	    .channelID = static_cast<ChannelID>(channelID.as<std::double_t>()),
	};

	SendUnreliable(data_);
}

/**
 * @file event/CoreEventsData.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "sol/string_view.hpp"
#include "sol/table.hpp"
#include "util/Definitions.hpp"

#include <cstdint>
#include <string_view>

struct _ENetPeer;

namespace tudov
{
	enum class EDisconnectionCode : std::uint32_t;
	enum class ESocketType : std::uint8_t;
	class Context;
	class IDebugManager;

	struct CoreEventData
	{
	};

	struct EventDebugProvideData : public CoreEventData
	{
		Context &context;
		IDebugManager &debugManager;

		explicit EventDebugProvideData(Context &context, IDebugManager &debugManager) noexcept;

		void LuaAddElement(sol::table args) noexcept;

		void LuaSetDebugCommand(sol::table args) noexcept;
	};

	struct EventLocalClientConnectData : public CoreEventData
	{
		ESocketType socketType;
		NetworkSessionSlot clientSlot;
		NetworkSessionSlot serverSlot;
	};

	struct EventLocalClientDisconnectData : public CoreEventData
	{
		ESocketType socketType;
		EDisconnectionCode code;
		ClientSessionID clientID;
		NetworkSessionSlot clientSlot;
		NetworkSessionSlot serverSlot;
	};

	struct EventLocalClientMessageData : public CoreEventData
	{
		ESocketType socketType;
		ClientSessionID clientID;
		std::string_view message;
		NetworkSessionSlot clientSlot;
		NetworkSessionSlot serverSlot;
	};

	struct EventLocalServerConnectData : public CoreEventData
	{
		ESocketType socketType;
		ClientSessionID clientID;
		NetworkSessionSlot clientSlot;
		NetworkSessionSlot serverSlot;
	};

	struct EventLocalServerDisconnectData : public CoreEventData
	{
		ESocketType socketType;
		EDisconnectionCode code;
		NetworkSessionSlot clientSlot;
		NetworkSessionSlot serverSlot;
	};

	struct EventLocalServerHostData : public CoreEventData
	{
		ESocketType socketType;
		NetworkSessionSlot serverSlot;
	};

	struct EventLocalServerMessageData : public CoreEventData
	{
		ESocketType socketType;
		ClientSessionID clientID;
		std::string_view message;
		sol::string_view broadcast;
		NetworkSessionSlot clientSlot;
		NetworkSessionSlot serverSlot;
	};

	struct EventLocalServerShutdownData : public CoreEventData
	{
		ESocketType socketType;
		NetworkSessionSlot serverSlot;
	};

	struct EventReliableUDPClientConnectData : public CoreEventData
	{
		ESocketType socketType;
		std::string_view host;
		std::uint16_t port;
	};

	struct EventReliableUDPClientDisconnectData : public CoreEventData
	{
		ESocketType socketType;
		EDisconnectionCode code;
		std::string_view host;
		std::uint16_t port;
	};

	struct EventReliableUDPClientMessageData : public CoreEventData
	{
		ESocketType socketType;
		std::string_view message;
		std::string_view host;
		std::uint16_t port;
	};

	struct EventReliableUDPServerConnectData : public CoreEventData
	{
		ESocketType socketType;
		ClientSessionID clientID;
		std::string_view host;
		std::uint16_t port;
	};

	struct EventReliableUDPServerDisconnectData : public CoreEventData
	{
		ESocketType socketType;
		EDisconnectionCode code;
		ClientSessionID clientID;
		std::string_view host;
		std::uint16_t port;
	};

	struct EventReliableUDPServerHostData : public CoreEventData
	{
		ESocketType socketType;
		NetworkSessionSlot serverSlot;
	};

	struct EventReliableUDPServerMessageData : public CoreEventData
	{
		ESocketType socketType;
		ClientSessionID clientID;
		sol::string_view message;
		sol::string_view broadcast;
		std::string_view host;
		std::uint16_t port;
	};

	struct EventReliableUDPServerShutdownData : public CoreEventData
	{
		ESocketType socketType;
		NetworkSessionSlot serverSlot;
	};
} // namespace tudov

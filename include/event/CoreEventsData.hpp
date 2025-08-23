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
#include "util/Definitions.hpp"

#include <cstdint>
#include <string_view>

struct _ENetPeer;

namespace tudov
{
	enum class EDisconnectionCode : std::int32_t;
	enum class ESocketType : std::uint8_t;

	struct CoreEventData
	{
	};

	struct EventLocalClientConnectData : public CoreEventData
	{
		ESocketType socketType;
		std::uint32_t clientUID;
		std::uint32_t serverUID;
	};

	struct EventLocalClientDisconnectData : public CoreEventData
	{
		ESocketType socketType;
		ClientSessionToken clientToken;
		std::uint32_t clientUID;
		std::uint32_t serverUID;
	};

	struct EventLocalClientMessageData : public CoreEventData
	{
		ESocketType socketType;
		ClientSessionToken clientToken;
		std::string_view message;
		std::uint32_t clientUID;
		std::uint32_t serverUID;
	};

	struct EventLocalServerConnectData : public CoreEventData
	{
		ESocketType socketType;
		std::uint32_t clientUID;
		std::uint32_t serverUID;
		EDisconnectionCode disconnect;
	};

	struct EventLocalServerDisconnectData : public CoreEventData
	{
		ESocketType socketType;
		std::uint32_t clientUID;
		std::uint32_t serverUID;
	};

	struct EventLocalServerMessageData : public CoreEventData
	{
		ESocketType socketType;
		std::string_view message;
		std::uint32_t clientUID;
		std::uint32_t serverUID;
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
		ClientSessionToken clientToken;
		std::string_view host;
		std::uint16_t port;
		EDisconnectionCode disconnect;
	};

	struct EventReliableUDPServerDisconnectData : public CoreEventData
	{
		ESocketType socketType;
		ClientSessionToken clientToken;
		std::string_view host;
		std::uint16_t port;
	};

	struct EventReliableUDPServerMessageData : public CoreEventData
	{
		ESocketType socketType;
		ClientSessionToken clientToken;
		sol::string_view message;
		sol::string_view messageOverride;
		std::string_view host;
		std::uint16_t port;
	};
} // namespace tudov

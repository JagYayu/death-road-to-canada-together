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

#include "util/Definitions.hpp"

#include "sol/string_view.hpp"
#include "sol/table.hpp"

#include <cstdint>
#include <memory>
#include <string_view>

struct _ENetPeer;

namespace tudov
{
	enum class EDisconnectionCode : std::uint32_t;
	enum class EScanCode : std::int32_t;
	enum class EKeyCode : std::uint32_t;
	enum class EKeyModifier : std::uint32_t;
	enum class ESocketType : std::uint8_t;
	class Context;
	struct IDebugManager;
	struct IWindow;
	struct IKeyboard;

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

	struct EventModGlobalsIndexData : public CoreEventData
	{
		std::string_view scriptName;
		sol::object key;
		sol::object value;
	};

#pragma region Network

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
		ClientSessionID clientID;
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

#pragma endregion

	struct EventMouseMotionData : public CoreEventData
	{
		std::int32_t mouseID;
		std::float_t x;
		std::float_t y;
		std::float_t relativeX;
		std::float_t relativeY;
	};

	struct EventKeyData : public CoreEventData
	{
		std::shared_ptr<IWindow> window;
		std::uint32_t windowID;
		std::shared_ptr<IKeyboard> keyboard;
		std::uint32_t keyboardID;
		EScanCode scanCode;
		EKeyCode keyCode;
		EKeyModifier modifier;
	};

	struct EventWindowResizeData : public CoreEventData
	{
	};
} // namespace tudov

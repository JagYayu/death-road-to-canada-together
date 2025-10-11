/**
 * @file Event/CoreEventsData.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Util/Definitions.hpp"

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
	class ScriptModule;
	class DebugManager;
	class Window;
	class Keyboard;
	class Mouse;

	struct CoreEventData
	{
		static std::shared_ptr<CoreEventData> Extract(sol::object e) noexcept;

		template <typename T>
		static std::shared_ptr<T> Extract(sol::object e) noexcept
		{
			std::shared_ptr<CoreEventData> data = Extract(e);
			return data != nullptr ? std::static_pointer_cast<T>(data) : nullptr;
		}
	};

	struct EventDebugProvideData : public CoreEventData
	{
		Context &context;
		DebugManager &debugManager;

		explicit EventDebugProvideData(Context &context, DebugManager &debugManager) noexcept;

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
		std::shared_ptr<Window> window;
		WindowID windowID;
		std::shared_ptr<Mouse> mouse;
		MouseID mouseID;
		std::float_t x;
		std::float_t y;
		std::float_t relativeX;
		std::float_t relativeY;
	};

	struct EventKeyboardData : public CoreEventData
	{
		std::shared_ptr<Window> window;
		WindowID windowID;
		std::shared_ptr<Keyboard> keyboard;
		KeyboardID keyboardID;
		EScanCode scanCode;
		EKeyCode keyCode;
		EKeyModifier modifier;
	};

	struct EventKeyboardDeviceData : public CoreEventData
	{
		KeyboardID keyboardID;
	};

	struct EventWindowResizeData : public CoreEventData
	{
	};

	struct EventScriptUnloadData : public CoreEventData
	{
		ScriptID scriptID;
		std::string_view scriptName;
		sol::table module;
	};
} // namespace tudov

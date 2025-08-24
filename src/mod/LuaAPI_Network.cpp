/**
 * @file mod/LuaAPI_Network.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "event/CoreEventsData.hpp"
#include "mod/LuaAPI.hpp"

#include "network/ClientSession.hpp"
#include "network/NetworkManager.hpp"
#include "network/ServerSession.hpp"

using namespace tudov;

#define TE_ENUM(Class, ...)     lua.new_enum<Class>(#Class, __VA_ARGS__)
#define TE_USERTYPE(Class, ...) lua.new_usertype<Class>(#Class, __VA_ARGS__)

void LuaAPI::InstallNetwork(sol::state &lua, Context &context) noexcept
{
	TE_USERTYPE(EventLocalClientConnectData,
	            "socketType", &EventLocalClientConnectData::socketType,
	            "clientSlot", &EventLocalClientConnectData::clientSlot,
	            "serverSlot", &EventLocalClientConnectData::serverSlot);

	TE_USERTYPE(EventLocalClientDisconnectData,
	            "socketType", &EventLocalClientDisconnectData::socketType,
	            "clientID", &EventLocalClientDisconnectData::clientID,
	            "clientSlot", &EventLocalClientDisconnectData::clientSlot,
	            "serverSlot", &EventLocalClientDisconnectData::serverSlot);

	TE_USERTYPE(EventLocalClientMessageData,
	            "socketType", &EventLocalClientMessageData::socketType,
	            "clientID", &EventLocalClientMessageData::clientID,
	            "message", &EventLocalClientMessageData::message,
	            "clientSlot", &EventLocalClientMessageData::clientSlot,
	            "serverSlot", &EventLocalClientMessageData::serverSlot);

	TE_USERTYPE(EventLocalServerConnectData,
	            "socketType", &EventLocalServerConnectData::socketType,
	            "clientSlot", &EventLocalServerConnectData::clientSlot,
	            "serverSlot", &EventLocalServerConnectData::serverSlot,
	            "disconnect", &EventLocalServerConnectData::disconnect);

	TE_USERTYPE(EventLocalServerDisconnectData,
	            "socketType", &EventLocalServerDisconnectData::socketType,
	            "clientSlot", &EventLocalServerDisconnectData::clientSlot,
	            "serverSlot", &EventLocalServerDisconnectData::serverSlot);

	TE_USERTYPE(EventLocalServerMessageData,
	            "socketType", &EventLocalServerMessageData::socketType,
	            "message", &EventLocalServerMessageData::message,
	            "clientSlot", &EventLocalServerMessageData::clientSlot,
	            "serverSlot", &EventLocalServerMessageData::serverSlot);

	TE_USERTYPE(EventReliableUDPClientConnectData,
	            "socketType", &EventReliableUDPClientConnectData::socketType,
	            "host", &EventReliableUDPClientConnectData::host,
	            "port", &EventReliableUDPClientConnectData::port);

	TE_USERTYPE(EventReliableUDPClientDisconnectData,
	            "socketType", &EventReliableUDPClientDisconnectData::socketType,
	            "host", &EventReliableUDPClientDisconnectData::host,
	            "port", &EventReliableUDPClientDisconnectData::port);

	TE_USERTYPE(EventReliableUDPClientMessageData,
	            "socketType", &EventReliableUDPClientMessageData::socketType,
	            "message", &EventReliableUDPClientMessageData::message,
	            "host", &EventReliableUDPClientMessageData::host,
	            "port", &EventReliableUDPClientMessageData::port);

	TE_USERTYPE(EventReliableUDPServerConnectData,
	            "socketType", &EventReliableUDPServerConnectData::socketType,
	            "clientID", &EventReliableUDPServerConnectData::clientID,
	            "host", &EventReliableUDPServerConnectData::host,
	            "port", &EventReliableUDPServerConnectData::port,
	            "disconnect", &EventReliableUDPServerConnectData::disconnect);

	TE_USERTYPE(EventReliableUDPServerDisconnectData,
	            "socketType", &EventReliableUDPServerDisconnectData::socketType,
	            "clientID", &EventReliableUDPServerDisconnectData::clientID,
	            "host", &EventReliableUDPServerDisconnectData::host,
	            "port", &EventReliableUDPServerDisconnectData::port);

	TE_USERTYPE(EventReliableUDPServerMessageData,
	            "socketType", &EventReliableUDPServerMessageData::socketType,
	            "clientID", &EventReliableUDPServerMessageData::clientID,
	            "message", &EventReliableUDPServerMessageData::message,
	            "messageOverride", &EventReliableUDPServerMessageData::messageOverride,
	            "host", &EventReliableUDPServerMessageData::host,
	            "port", &EventReliableUDPServerMessageData::port);

	TE_USERTYPE(IClientSession,
	            "getSessionID", &IClientSession::GetSessionID,
	            "connect", &IClientSession::LuaConnect,
	            "disconnect", &IClientSession::Disconnect,
	            "getSessionState", &IClientSession::GetSessionState,
	            "sendReliable", &IClientSession::LuaSendReliable,
	            "sendUnreliable", &IClientSession::LuaSendUnreliable,
	            "tryDisconnect", &IClientSession::TryDisconnect);

	TE_USERTYPE(IServerSession,
	            "broadcastReliable", &IServerSession::LuaBroadcastReliable,
	            "broadcastUnreliable", &IServerSession::LuaBroadcastUnreliable,
	            "getSessionState", &IServerSession::GetSessionState,
	            "host", &IServerSession::LuaHost,
	            "sendReliable", &IServerSession::LuaSendReliable,
	            "sendUnreliable", &IServerSession::LuaSendUnreliable,
	            "shutdown", &IServerSession::Shutdown);
}

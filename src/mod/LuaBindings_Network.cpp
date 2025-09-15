/**
 * @file mod/LuaBindings_Network.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "mod/LuaBindings.hpp"

#include "event/CoreEventsData.hpp"
#include "network/ClientSession.hpp"
#include "network/ClientSessionState.hpp"
#include "network/DisconnectionCode.hpp"
#include "network/NetworkManager.hpp"
#include "network/ServerSession.hpp"
#include "network/ServerSessionState.hpp"

using namespace tudov;

#define TE_ENUM(Class, ...)     lua.new_enum<Class>(#Class, __VA_ARGS__)
#define TE_USERTYPE(Class, ...) lua.new_usertype<Class>(#Class, __VA_ARGS__)

void LuaBindings::InstallNetwork(sol::state &lua, Context &context) noexcept
{
	TE_ENUM(EClientSessionState,
	        {
	            {"Disconnected", EClientSessionState::Disconnected},
	            {"Connecting", EClientSessionState::Connecting},
	            {"Connected", EClientSessionState::Connected},
	            {"Disconnecting", EClientSessionState::Disconnecting},
	        });

	TE_ENUM(EDisconnectionCode,
	        {
	            {"Unknown", EDisconnectionCode::Unknown},
	            {"ClientClosed", EDisconnectionCode::ClientClosed},
	            {"ServerClosed", EDisconnectionCode::ServerClosed},
	        });

	TE_ENUM(EServerSessionState,
	        {
	            {"Shutdown", EServerSessionState::Shutdown},
	            {"Starting", EServerSessionState::Starting},
	            {"Hosting", EServerSessionState::Hosting},
	            {"Stopping", EServerSessionState::Stopping},
	        });

	TE_ENUM(ESocketType,
	        {
	            {"Local", ESocketType::Local},
	            {"RUDP", ESocketType::RUDP},
	            {"Steam", ESocketType::Steam},
	            {"TCP", ESocketType::TCP},
	        });

	TE_ENUM(ESocketType,
	        {
	            {"Local", ESocketType::Local},
	            {"RUDP", ESocketType::RUDP},
	            {"Steam", ESocketType::Steam},
	            {"TCP", ESocketType::TCP},
	        });

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
	            "clientID", &EventLocalServerConnectData::clientID,
	            "clientSlot", &EventLocalServerConnectData::clientSlot,
	            "serverSlot", &EventLocalServerConnectData::serverSlot);

	TE_USERTYPE(EventLocalServerDisconnectData,
	            "socketType", &EventLocalServerDisconnectData::socketType,
	            "clientSlot", &EventLocalServerDisconnectData::clientSlot,
	            "serverSlot", &EventLocalServerDisconnectData::serverSlot);

	TE_USERTYPE(EventLocalServerMessageData,
	            "clientID", &EventLocalServerMessageData::clientID,
	            "clientSlot", &EventLocalServerMessageData::clientSlot,
	            "message", &EventLocalServerMessageData::message,
	            "serverSlot", &EventLocalServerMessageData::serverSlot,
	            "socketType", &EventLocalServerMessageData::socketType);

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
	            "port", &EventReliableUDPServerConnectData::port);

	TE_USERTYPE(EventReliableUDPServerDisconnectData,
	            "socketType", &EventReliableUDPServerDisconnectData::socketType,
	            "clientID", &EventReliableUDPServerDisconnectData::clientID,
	            "host", &EventReliableUDPServerDisconnectData::host,
	            "port", &EventReliableUDPServerDisconnectData::port);

	TE_USERTYPE(EventReliableUDPServerMessageData,
	            "socketType", &EventReliableUDPServerMessageData::socketType,
	            "clientID", &EventReliableUDPServerMessageData::clientID,
	            "message", &EventReliableUDPServerMessageData::message,
	            "messageOverride", &EventReliableUDPServerMessageData::broadcast,
	            "host", &EventReliableUDPServerMessageData::host,
	            "port", &EventReliableUDPServerMessageData::port);

	TE_USERTYPE(IClientSession,
	            "getSessionID", &IClientSession::GetSessionID,
	            "getSocketType", &IClientSession::GetSocketType,
	            "connect", &IClientSession::LuaConnect,
	            "disconnect", &IClientSession::LuaDisconnect,
	            "getSessionState", &IClientSession::GetSessionState,
	            "sendReliable", &IClientSession::LuaSendReliable,
	            "sendUnreliable", &IClientSession::LuaSendUnreliable,
	            "tryDisconnect", &IClientSession::TryDisconnect);

	TE_USERTYPE(IServerSession,
	            "broadcastReliable", &IServerSession::LuaBroadcastReliable,
	            "broadcastUnreliable", &IServerSession::LuaBroadcastUnreliable,
	            "disconnect", &IServerSession::LuaDisconnect,
	            "getClients", &IServerSession::GetClients,
	            "getMaxClients", &IServerSession::GetMaxClients,
	            "getPassword", &IServerSession::GetPassword,
	            "getSessionState", &IServerSession::GetSessionState,
	            "getSocketType", &IClientSession::GetSocketType,
	            "getTitle", &IServerSession::GetTitle,
	            "host", &IServerSession::LuaHost,
	            "sendReliable", &IServerSession::LuaSendReliable,
	            "sendUnreliable", &IServerSession::LuaSendUnreliable,
	            "shutdown", &IServerSession::Shutdown);

	TE_USERTYPE(NetworkManager,
	            "clientConnectRUDP", &NetworkManager::LuaClientConnectRUDP,
	            "clientDisconnect", &NetworkManager::LuaClientDisconnect,
	            "getClient", &NetworkManager::LuaGetClient,
	            "getServer", &NetworkManager::LuaGetServer,
	            "serverHostRUDP", &NetworkManager::LuaServerHostRUDP,
	            "serverShutdown", &NetworkManager::LuaServerShutdown,
	            "update", &NetworkManager::Update);

	lua["network"] = &dynamic_cast<NetworkManager &>(context.GetNetworkManager());
}

/**
 * @file network/NetworkManager.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Network/NetworkManager.hpp"

#include "Exception/BadEnumException.hpp"
#include "Mod/ScriptEngine.hpp"
#include "Network/LocalClientSession.hpp"
#include "Network/LocalServerSession.hpp"
#include "Network/ReliableUDPClientSession.hpp"
#include "Network/ReliableUDPServerSession.hpp"
#include "Network/SocketType.hpp"
#include "Util/Definitions.hpp"
#include "Util/Utils.hpp"

#include "sol/types.hpp"

#include <cmath>
#include <format>
#include <memory>
#include <string_view>
#include <vector>

using namespace tudov;

std::int32_t INetworkManager::GetLimitsPerUpdate() noexcept
{
	return 4;
}

bool INetworkManager::Update() noexcept
{
	std::int32_t limit = GetLimitsPerUpdate();
	if (limit <= 0)
	{
		return true;
	}

	auto &&clients = GetClients();
	auto &&servers = GetServers();
	bool update = true;
	while (update)
	{
		if (limit <= 0)
		{
			return true;
		}
		--limit;

		update = false;
		for (auto &&client : clients)
		{
			if (!client.expired() && client.lock()->Update())
			{
				update = true;
			}
		}
		for (auto &&server : servers)
		{
			if (!server.expired() && server.lock()->Update())
			{
				update = true;
			}
		}
	}

	return false;
}

NetworkManager::NetworkManager(Context &context) noexcept
    : _log(Log::Get("NetworkManager")),
      _context(context),
      _initialized(false),
      _socketType(ESocketType::Local),
      _debugClientSlot(0),
      _debugServerSlot(0)
{
}

Log &NetworkManager::GetLog() noexcept
{
	return *_log;
}

Context &NetworkManager::GetContext() noexcept
{
	return _context;
}

void NetworkManager::Initialize() noexcept
{
	if (_initialized)
	{
		return;
	}

	_socketType = ESocketType::Local;
	_initialized = true;
}

void NetworkManager::Deinitialize() noexcept
{
	if (!_initialized)
	{
		return;
	}

	_clients.clear();
	_servers.clear();

	_initialized = false;
}

IClientSession *NetworkManager::GetClient(NetworkSessionSlot clientSlot) noexcept
{
	auto it = _clients.find(clientSlot);
	return it == _clients.end() ? nullptr : it->second.get();
}

IServerSession *NetworkManager::GetServer(NetworkSessionSlot serverSlot) noexcept
{
	auto it = _servers.find(serverSlot);
	return it == _servers.end() ? nullptr : it->second.get();
}

std::vector<std::weak_ptr<IClientSession>> NetworkManager::GetClients() noexcept
{
	std::vector<std::weak_ptr<IClientSession>> clients{_clients.size()};
	for (auto &&entry : _clients)
	{
		clients.emplace_back(entry.second);
	}
	return std::move(clients);
}

std::vector<std::weak_ptr<IServerSession>> NetworkManager::GetServers() noexcept
{
	std::vector<std::weak_ptr<IServerSession>> servers{_servers.size()};
	for (auto &&entry : _servers)
	{
		servers.emplace_back(entry.second);
	}
	return std::move(servers);
}

bool NetworkManager::SetClient(NetworkSessionSlot clientSlot)
{
	// TODO should call `client::TryDisconnect`.
	return _clients.erase(clientSlot);
}

bool NetworkManager::SetClient(ESocketType socketType, NetworkSessionSlot clientSlot)
{
	if (auto it = _clients.find(clientSlot); it != _clients.end() && it->second != nullptr)
	{
		if (it->second->GetSocketType() == socketType)
		{
			return false;
		}

		it->second->TryDisconnect(EDisconnectionCode::Unknown);
	}

	switch (socketType)
	{
	case ESocketType::Local:
		_clients[clientSlot] = std::make_shared<LocalClientSession>(*this, clientSlot);
		break;
	case ESocketType::RUDP:
		_clients[clientSlot] = std::make_shared<ReliableUDPClientSession>(*this, clientSlot);
		break;
	case ESocketType::Steam:
		// _clients[uid] = std::make_shared<SteamClient>(*this);
		// break;
	[[unlikely]] default:
		throw BadEnumException<ESocketType>(_context, socketType);
	}

	return true;
}

bool NetworkManager::SetServer(NetworkSessionSlot serverSlot)
{
	// TODO should call `server::TryShutdown`.
	return _servers.erase(serverSlot);
}

bool NetworkManager::SetServer(ESocketType socketType, NetworkSessionSlot serverSlot)
{
	if (auto it = _servers.find(serverSlot); it != _servers.end() && it->second != nullptr)
	{
		if (it->second->GetSocketType() == socketType)
		{
			return false;
		}

		it->second->TryShutdown();
	}

	switch (socketType)
	{
	case ESocketType::Local:
		_servers[serverSlot] = std::make_shared<LocalServerSession>(*this, serverSlot);
		break;
	case ESocketType::RUDP:
		_servers[serverSlot] = std::make_shared<ReliableUDPServerSession>(*this, serverSlot);
		break;
	case ESocketType::Steam:
		// TODO
		// _servers[uid] = std::make_shared<SteamServer>(*this);
		// break;
	[[unlikely]] default:
		throw BadEnumException<ESocketType>(_context, socketType);
	}

	return true;
}

bool NetworkManager::Update() noexcept
{
	if (INetworkManager::Update())
	{
		_log->Warn("Update rate times has achieved limit {}, probably has infinite loop?", GetLimitsPerUpdate());

		return true;
	}
	else
	{
		return false;
	}
}

void NetworkManager::LuaServerHostRUDP(sol::object args) noexcept
{
	if (!args.is<sol::table>())
	{
		GetScriptEngine().ThrowError("Type mismatch, table expected got {}", GetLuaTypeStringView(args.get_type()));
	}
	auto tbl = args.as<sol::table>();

	IServerSession *server;
	{
		std::uint32_t slot = tbl.get_or<std::double_t>("slot", 0);

		SetServer(ESocketType::RUDP, slot);

		server = GetServer(slot);
	}

	if (server->GetSocketType() != ESocketType::RUDP)
	{
		//
	}

	try
	{
		ReliableUDPServerSession::HostArgs args;
		args.host = tbl.get_or<sol::string_view>("host", "");
		args.maximumClients = tbl.get_or<std::double_t>("maximumClients", NetworkServerMaximumClients);
		args.password = tbl.get_or<sol::string_view>("password", NetworkServerPassword);
		args.port = tbl.get_or<std::double_t>("port", 0);
		args.title = tbl.get_or<sol::string_view>("password", NetworkServerTitle);
		server->Host(args);
	}
	catch (const std::exception &e)
	{
		GetScriptEngine().ThrowError("{}", e.what());
	}
}

void NetworkManager::LuaServerShutdown(sol::object args) noexcept
{
	try
	{
		//
	}
	catch (const std::exception &e)
	{
		//
	}
}

void NetworkManager::LuaClientConnectRUDP(sol::object args) noexcept
{
	if (!args.is<sol::table>())
	{
		GetScriptEngine().ThrowError("Type mismatch, table expected got {}", GetLuaTypeStringView(args.get_type()));
	}
	auto tbl = args.as<sol::table>();

	IClientSession *client;
	{
		std::uint32_t slot = tbl.get_or<std::double_t>("slot", 0);

		SetClient(ESocketType::RUDP, slot);

		client = GetClient(slot);
	}

	try
	{
		ReliableUDPClientSession::ConnectArgs args;
		args.host = tbl.get_or<sol::string_view>("host", "");
		args.password = tbl.get_or<sol::string_view>("password", NetworkServerPassword);
		args.port = tbl.get_or<std::double_t>("port", 0);
		client->Connect(args);
	}
	catch (const std::exception &e)
	{
		GetScriptEngine().ThrowError("{}", e.what());
	}
}

void NetworkManager::LuaClientDisconnect(sol::object args) noexcept
{
	try
	{
		//
	}
	catch (const std::exception &e)
	{
		//
	}
}

IClientSession *NetworkManager::LuaGetClient(sol::object uid) noexcept
{
	if (uid.is<sol::nil_t>())
	{
		return GetClient();
	}
	else if (uid.is<std::double_t>())
	{
		return GetClient(uid.as<std::double_t>());
	}
	else [[unlikely]]
	{
		GetScriptEngine().ThrowError("bad argument #1 to 'uid' (number or nil expected, got {})", GetLuaTypeStringView(uid.get_type()));
	}
}

IServerSession *NetworkManager::LuaGetServer(sol::object uid) noexcept
{
	if (uid.is<sol::nil_t>())
	{
		return GetServer();
	}
	else if (uid.is<std::double_t>())
	{
		return GetServer(uid.as<std::double_t>());
	}
	else [[unlikely]]
	{
		GetScriptEngine().ThrowError("bad argument #1 to 'uid' (number or nil expected, got {})", GetLuaTypeStringView(uid.get_type()));
	}
}

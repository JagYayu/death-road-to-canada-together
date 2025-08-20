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

#include "network/NetworkManager.hpp"

#include "debug/DebugConsole.hpp"
#include "debug/DebugManager.hpp"
#include "exception/BadEnumException.hpp"
#include "exception/Exception.hpp"
#include "network/LocalClient.hpp"
#include "network/LocalServer.hpp"
#include "network/ReliableUDPClient.hpp"
#include "network/ReliableUDPServer.hpp"
#include "network/SocketType.hpp"
#include "sol/types.hpp"
#include "util/Utils.hpp"

#include <cmath>
#include <format>
#include <memory>
#include <string>
#include <vector>

using namespace tudov;

std::int32_t INetworkManager::GetLimitsPerUpdate() noexcept
{
	return 3;
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
    : _log(Log::Get("Network")),
      _context(context),
      _initialized(false),
      _socketType(ESocketType::Local),
      _debugClientUID(0),
      _debugServerUID(0)
{
}

Log &NetworkManager::GetLog() noexcept
{
	return *_log;
}

std::vector<DebugConsoleResult> NetworkManager::DebugClientConnect(std::string_view arg) noexcept
{
	std::vector<DebugConsole::Result> results{};

	auto &&client = GetClient(_debugClientUID);
	if (client == nullptr)
	{
		results.emplace_back(std::format("Client with uid {} does not exist", _debugClientUID), DebugConsole::Code::Failure);
		return results;
	}

	std::string_view address;
	{
		auto spacePos = arg.find(' ');
		if (spacePos == std::string_view::npos)
		{
			results.emplace_back("Bad command", DebugConsole::Code::Failure);
			return results;
		}

		auto socket = arg.substr(0, spacePos);
		auto &&socketType = StringToSocketType(socket);
		if (!socketType.has_value())
		{
			results.emplace_back("Socket not provided or invalid", DebugConsole::Code::Failure);
			return results;
		}

		try
		{
			SetClient(socketType.value(), _debugClientUID);
		}
		catch (const Exception &e)
		{
			results.emplace_back(e.what(), DebugConsole::Code::Failure);
			return results;
		}

		address = arg.substr(spacePos + 1);
		address.remove_prefix(std::min(address.find_first_not_of(" "), address.size()));
	}

	if (auto &&localClient = dynamic_cast<LocalClient *>(client); localClient != nullptr)
	{
		LocalServer *server = nullptr;
		try
		{
			server = dynamic_cast<LocalServer *>(GetServer(std::stoi(address.data())));
		}
		catch (std::exception &)
		{
		}

		LocalClient::ConnectArgs args;
		args.user = _debugClientUID;
		args.server = server;
		localClient->Connect(args);

		results.emplace_back("Connecting to local server", DebugConsole::Code::Success);
	}
	else if (auto &&reliableUDPClient = dynamic_cast<ReliableUDPClient *>(client); reliableUDPClient != nullptr)
	{
		std::istringstream iss{std::string(address)};
		std::string host;
		std::string port;
		iss >> host >> port;

		ReliableUDPClient::ConnectArgs args;
		args.host = host;
		args.port = std::stoi(port);
		reliableUDPClient->Connect(args);

		results.emplace_back(std::format("Connecting to reliable udp server {} {}", host, port), DebugConsole::Code::Success);
	}
	else
	{
		results.emplace_back("Unhandled client connecting logic", DebugConsole::Code::Failure);
	}

	return results;
}

std::vector<DebugConsoleResult> NetworkManager::DebugServerHost(std::string_view arg) noexcept
{
	std::vector<DebugConsole::Result> results{};

	std::string_view address;
	{
		auto spacePos = arg.find(' ');
		if (spacePos == std::string_view::npos)
		{
			results.emplace_back("Bad command", DebugConsole::Code::Failure);
			return results;
		}

		auto socket = arg.substr(0, spacePos);
		auto &&socketType = StringToSocketType(socket);
		if (!socketType.has_value())
		{
			results.emplace_back("Socket not provided or invalid", DebugConsole::Code::Failure);
			return results;
		}

		try
		{
			SetServer(socketType.value(), _debugServerUID);
		}
		catch (const Exception &e)
		{
			results.emplace_back(e.what(), DebugConsole::Code::Failure);
			return results;
		}

		address = arg.substr(spacePos + 1);
		address.remove_prefix(std::min(address.find_first_not_of(" "), address.size()));
	}

	IServer *server = GetServer(_debugServerUID);
	if (server == nullptr) [[unlikely]]
	{
		results.emplace_back(std::format("Server with uid {} does not exist", _debugClientUID), DebugConsole::Code::Failure);
		return results;
	}

	if (auto &&localServer = dynamic_cast<LocalServer *>(server); localServer != nullptr)
	{
		LocalServer *server = nullptr;
		try
		{
			server = dynamic_cast<LocalServer *>(GetServer(std::stoi(address.data())));
		}
		catch (std::exception &)
		{
		}

		LocalServer::HostArgs args;
		localServer->Host(args);

		results.emplace_back("Hosting local server", DebugConsole::Code::Success);
	}
	else if (auto &&reliableUDPServer = dynamic_cast<ReliableUDPServer *>(server); reliableUDPServer != nullptr)
	{
		std::istringstream iss{std::string(address)};
		std::string host;
		std::string port;
		iss >> host >> port;

		ReliableUDPServer::HostArgs args;
		reliableUDPServer->Host(args);

		results.emplace_back(std::format("Connecting to reliable udp server {} {}", host, port), DebugConsole::Code::Success);
	}
	else
	{
		results.emplace_back("Unhandled server hosting logic", DebugConsole::Code::Failure);
	}

	return results;
}

void NetworkManager::ProvideDebug(IDebugManager &debugManager) noexcept
{
	if (DebugConsole *console = debugManager.GetElement<DebugConsole>(); console != nullptr)
	{
		auto &&clientConnect = [this](std::string_view arg)
		{
			return DebugClientConnect(arg);
		};

		console->SetCommand(DebugConsole::Command{
		    .name = "clientConnect",
		    .help = "clientConnect <socket> <address>: Connect current client to a server.",
		    .func = clientConnect,
		});

		auto &&clientConnection = [this](std::string_view arg)
		{
			std::vector<DebugConsole::Result> results{};

			return results;
		};

		console->SetCommand(DebugConsole::Command{
		    .name = "clientConnection",
		    .help = "clientConnection [uid]: Check client's connection info.",
		    .func = clientConnection,
		});

		auto &&clientSet = [this](std::string_view arg)
		{
			auto prevUID = _debugClientUID;
			_debugClientUID = std::stoi(arg.data());

			std::vector<DebugConsole::Result> results{};
			results.emplace_back(DebugConsole::Result{
			    .message = std::format("Changed client uid from {} to {}", prevUID, _debugClientUID),
			    .code = DebugConsole::Code::Success,
			});
			return results;
		};

		console->SetCommand(DebugConsole::Command{
		    .name = "clientSet",
		    .help = "clientSet <uid>: Change current client to specific uid.",
		    .func = clientSet,
		});

		auto &&serverHost = [this](std::string_view arg)
		{
			return DebugServerHost(arg);
		};

		console->SetCommand(DebugConsole::Command{
		    .name = "serverHost",
		    .help = "serverHost <socket> <address>: Host server.",
		    .func = serverHost,
		});

		auto &&serverInfo = [this](std::string_view arg)
		{
			std::vector<DebugConsole::Result> results{};
			return results;
		};

		console->SetCommand(DebugConsole::Command{
		    .name = "serverInfo",
		    .help = "serverInfo [uid]: Check server's hosting info.",
		    .func = serverInfo,
		});
	}
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

	auto &&client = std::make_shared<LocalClient>(*this);
	auto &&server = std::make_shared<LocalServer>(*this);

	_clients[0] = client;
	_servers[0] = server;

	LocalServer::HostArgs hostArgs;
	hostArgs.title = "Local Server";
	hostArgs.maximumClients = -1;
	server->Host(hostArgs);

	LocalClient::ConnectArgs connectArgs;
	connectArgs.server = server.get();
	connectArgs.user = 0;
	client->Connect(connectArgs);

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

IClient *NetworkManager::GetClient(std::int32_t uid) noexcept
{
	auto &&it = _clients.find(uid);
	return it == _clients.end() ? nullptr : it->second.get();
}

IServer *NetworkManager::GetServer(std::int32_t uid) noexcept
{
	auto &&it = _servers.find(uid);
	return it == _servers.end() ? nullptr : it->second.get();
}

std::vector<std::weak_ptr<IClient>> NetworkManager::GetClients() noexcept
{
	std::vector<std::weak_ptr<IClient>> clients{_clients.size()};
	for (auto &&entry : _clients)
	{
		clients.emplace_back(entry.second);
	}
	return std::move(clients);
}

std::vector<std::weak_ptr<IServer>> NetworkManager::GetServers() noexcept
{
	std::vector<std::weak_ptr<IServer>> servers{_servers.size()};
	for (auto &&entry : _servers)
	{
		servers.emplace_back(entry.second);
	}
	return std::move(servers);
}

bool NetworkManager::SetClient(std::int32_t uid)
{
	return _clients.erase(uid);
}

bool NetworkManager::SetClient(ESocketType socketType, std::int32_t uid)
{
	if (auto &&it = _clients.find(uid); it != _clients.end() && it->second->GetSocketType() == socketType)
	{
		return false;
	}

	switch (socketType)
	{
	case ESocketType::Local:
		_clients[uid] = std::make_shared<LocalClient>(*this);
		break;
	case ESocketType::RUDP:
		_clients[uid] = std::make_shared<ReliableUDPClient>(*this);
		break;
	case ESocketType::Steam:
		// _clients[uid] = std::make_shared<SteamClient>(*this);
		// break;
	[[unlikely]] default:
		throw BadEnumException<ESocketType>(_context, socketType);
	}

	return true;
}

bool NetworkManager::SetServer(std::int32_t uid)
{
	return _servers.erase(uid);
}

bool NetworkManager::SetServer(ESocketType socketType, std::int32_t uid)
{
	if (auto &&it = _servers.find(uid); it != _servers.end() && it->second->GetSocketType() == socketType)
	{
		return false;
	}

	switch (socketType)
	{
	case ESocketType::Local:
		_servers[uid] = std::make_shared<LocalServer>(*this);
		break;
	case ESocketType::RUDP:
		_servers[uid] = std::make_shared<ReliableUDPServer>(*this);
		break;
	case ESocketType::Steam:
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
	return false;
}

IClient *NetworkManager::LuaGetClient(sol::object uid) noexcept
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

IServer *NetworkManager::LuaGetServer(sol::object uid) noexcept
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

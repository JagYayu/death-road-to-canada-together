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
#include "mod/ScriptEngine.hpp"
#include "network/LocalClientSession.hpp"
#include "network/LocalServerSession.hpp"
#include "network/NetworkSessionData.hpp"
#include "network/ReliableUDPClientSession.hpp"
#include "network/ReliableUDPServerSession.hpp"
#include "network/SocketType.hpp"
#include "util/Definitions.hpp"
#include "util/Utils.hpp"

#include "sol/types.hpp"

#include <cmath>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

using namespace tudov;

std::int32_t INetworkManager::GetLimitsPerUpdate() noexcept
{
	return 32;
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

std::vector<DebugConsoleResult> NetworkManager::DebugClientConnect(std::string_view arg) noexcept
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

		std::string_view socket = arg.substr(0, spacePos);
		auto optSocketType = StringToSocketType(socket);
		if (!optSocketType.has_value())
		{
			results.emplace_back("Socket not provided or invalid", DebugConsole::Code::Failure);
			return results;
		}

		try
		{
			SetClient(optSocketType.value(), _debugClientSlot);
		}
		catch (const Exception &e)
		{
			results.emplace_back(std::format("Failed to set client socket: {}", e.what()), DebugConsole::Code::Failure);
			return results;
		}

		address = arg.substr(spacePos + 1);
		address.remove_prefix(std::min(address.find_first_not_of(" "), address.size()));
	}

	IClientSession *client = GetClient(_debugClientSlot);
	if (auto *localClient = dynamic_cast<LocalClientSession *>(client); localClient != nullptr)
	{
		try
		{
			LocalServerSession *server = nullptr;
			try
			{
				server = dynamic_cast<LocalServerSession *>(GetServer(std::stoi(address.data())));
			}
			catch (std::exception &)
			{
			}

			LocalClientSession::ConnectArgs args;
			args.localServer = server;

			localClient->Connect(args);
			results.emplace_back("Connecting to local server", DebugConsole::Code::Success);
		}
		catch (const std::exception &e)
		{
			results.emplace_back(e.what(), DebugConsole::Code::Failure);
		}
	}
	else if (auto *reliableUDPClient = dynamic_cast<ReliableUDPClientSession *>(client); reliableUDPClient != nullptr)
	{
		try
		{
			std::istringstream iss{std::string(address)};
			std::string host;
			std::string port;
			iss >> host >> port;

			ReliableUDPClientSession::ConnectArgs args;
			args.host = host;
			args.port = std::stoi(port);

			reliableUDPClient->Connect(args);

			results.emplace_back(std::format("Connecting to reliable udp server {} {}", host, port), DebugConsole::Code::Success);
		}
		catch (const std::exception &e)
		{
			results.emplace_back(e.what(), DebugConsole::Code::Failure);
		}
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
			SetServer(socketType.value(), _debugServerSlot);
		}
		catch (const Exception &e)
		{
			results.emplace_back(e.what(), DebugConsole::Code::Failure);
			return results;
		}

		address = arg.substr(spacePos + 1);
		address.remove_prefix(std::min(address.find_first_not_of(" "), address.size()));
	}

	IServerSession *server = GetServer(_debugServerSlot);
	if (auto &&localServer = dynamic_cast<LocalServerSession *>(server); localServer != nullptr)
	{
		LocalServerSession::HostArgs args;
		localServer->Host(args);

		results.emplace_back("Hosting local server", DebugConsole::Code::Success);
	}
	else if (auto &&reliableUDPServer = dynamic_cast<ReliableUDPServerSession *>(server); reliableUDPServer != nullptr)
	{
		std::istringstream iss{std::string(address)};
		std::string host;
		std::string port;
		iss >> host >> port;

		ReliableUDPServerSession::HostArgs args;
		args.host = host;
		args.port = std::stoi(port);
		reliableUDPServer->Host(args);

		results.emplace_back(std::format("Hosting reliable udp server {} {}", host, port), DebugConsole::Code::Success);
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

		auto &&clientInfo = [this](std::string_view arg)
		{
			std::vector<DebugConsole::Result> results{};

			return results;
		};

		console->SetCommand(DebugConsole::Command{
		    .name = "clientInfo",
		    .help = "clientInfo [uid]: Check client's connection info.",
		    .func = clientInfo,
		});

		auto &&clientSet = [this](std::string_view arg)
		{
			NetworkSessionSlot prevUID = _debugClientSlot;
			_debugClientSlot = std::stoi(arg.data());

			std::vector<DebugConsole::Result> results{};
			results.emplace_back(DebugConsole::Result{
			    .message = std::format("Changed client uid from {} to {}", prevUID, _debugClientSlot),
			    .code = DebugConsole::Code::Success,
			});
			return results;
		};

		console->SetCommand(DebugConsole::Command{
		    .name = "clientSet",
		    .help = "clientSet <uid>: Change current client to specific uid.",
		    .func = clientSet,
		});

		auto &&clientSend = [this](std::string_view arg)
		{
			std::vector<DebugConsole::Result> results;

			try
			{
				NetworkSessionData data{
				    .bytes = std::span<const std::byte>(reinterpret_cast<const std::byte *>(arg.data()), arg.size()),
				    .channelID = 0,
				};

				const std::shared_ptr<IClientSession> &client = _clients.at(_debugClientSlot);

				client->SendReliable(data);

				auto &&msg = std::format("Client<{}>{} sent {} message to server: {}",
				                         client->GetSessionSlot(), client->GetSessionID(), "reliable", arg);
				results.emplace_back(msg, DebugConsole::Code::Success);
			}
			catch (const std::exception &e)
			{
				results.emplace_back(DebugConsole::Result{
				    .message = std::format("{}", e.what()),
				    .code = DebugConsole::Code::Failure,
				});
			}

			return results;
		};

		console->SetCommand(DebugConsole::Command{
		    .name = "clientSend",
		    .help = "clientSend <message> [unreliable]: Send reliable/unreliable message to server",
		    .func = clientSend,
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

	// auto &&client = std::make_shared<LocalClientSession>(*this, DefaultSessionSlot);
	// auto &&server = std::make_shared<LocalServerSession>(*this, DefaultSessionSlot);

	// _clients[DefaultSessionSlot] = client;
	// _servers[DefaultSessionSlot] = server;

	// LocalServerSession::HostArgs hostArgs;
	// hostArgs.title = "Local Server";
	// hostArgs.maximumClients = -1;
	// server->Host(hostArgs);

	// LocalClientSession::ConnectArgs connectArgs;
	// connectArgs.localServer = server.get();
	// client->Connect(connectArgs);

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

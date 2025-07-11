#include "Network.hpp"

#include "LocalClient.hpp"
#include "LocalServer.hpp"
#include "SocketType.hpp"
#include "debug/DebugManager.hpp"

#include <cstdlib>
#include <format>
#include <memory>
#include <string>
#include <vector>

using namespace tudov;

std::int32_t INetwork::GetLimitsPerUpdate() noexcept
{
	return 3;
}

bool INetwork::Update() noexcept
{
	auto &&client = GetClient();
	auto &&server = GetServer();

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

Network::Network(Context &context) noexcept
    : _log(Log::Get("Network")),
      _context(context),
      _initialized(false),
      _socketType(ESocketType::Local)
{
}

Log &Network::GetLog() noexcept
{
	return *_log;
}

void Network::ProvideDebug(IDebugManager &debugManager) noexcept
{
	if (auto &&console = debugManager.GetElement<DebugConsole>(); console != nullptr)
	{
		auto &&clientConnect = [this](std::string_view arg)
		{
			std::vector<DebugConsole::Result> results{};

			auto &&client = GetClient(_debugClientUID);
			if (client == nullptr)
			{
				results.emplace_back(std::format("Client with uid {} does not exist", _debugClientUID), DebugConsole::Code::Failure);
				return results;
			}

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
				results.emplace_back("Socket not provided", DebugConsole::Code::Failure);
				return results;
			}
			SetClient(socketType.value(), _debugClientUID);

			auto address = arg.substr(spacePos + 1);
			address.remove_prefix(std::min(address.find_first_not_of(" "), address.size()));

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
			}

			return results;
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
			std::vector<DebugConsole::Result> results{};

			return results;
		};

		console->SetCommand(DebugConsole::Command{
		    .name = "serverHost",
		    .help = "serverHost <socket> <address>: Host server.",
		    .func = serverHost,
		});
	}
}

Context &Network::GetContext() noexcept
{
	return _context;
}

void Network::Initialize() noexcept
{
	if (_initialized)
	{
		return;
	}

	auto &&client = std::make_shared<LocalClient>(*this);
	auto &&server = std::make_shared<LocalServer>(*this);

	_clients.try_emplace(0, client);
	_servers.try_emplace(0, server);

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

void Network::Deinitialize() noexcept
{
	if (!_initialized)
	{
		return;
	}

	_clients.clear();
	_servers.clear();

	_initialized = false;
}

IClient *Network::GetClient(std::int32_t uid) noexcept
{
	auto &&it = _clients.find(uid);
	return it == _clients.end() ? nullptr : it->second.get();
}

IServer *Network::GetServer(std::int32_t uid) noexcept
{
	auto &&it = _servers.find(uid);
	return it == _servers.end() ? nullptr : it->second.get();
}

std::vector<std::weak_ptr<IClient>> Network::GetClients() noexcept
{
	std::vector<std::weak_ptr<IClient>> clients{_clients.size()};
	for (auto &&entry : _clients)
	{
		clients.emplace_back(entry.second);
	}
	return std::move(clients);
}

std::vector<std::weak_ptr<IServer>> Network::GetServers() noexcept
{
	std::vector<std::weak_ptr<IServer>> servers{_servers.size()};
	for (auto &&entry : _servers)
	{
		servers.emplace_back(entry.second);
	}
	return std::move(servers);
}

void Network::SetClient(ESocketType socketType, std::int32_t uid)
{
}

void Network::SetServer(ESocketType socketType, std::int32_t uid)
{
}

bool Network::Update() noexcept
{
	if (INetwork::Update())
	{
		_log->Warn("Update rate times has achieved limit {}, probably has infinite loop?", GetLimitsPerUpdate());
		return true;
	}
	return false;
}

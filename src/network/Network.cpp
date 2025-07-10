#include "Network.hpp"

#include "LocalClient.hpp"
#include "LocalServer.hpp"

#include <memory>

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

Network::Network(Context &context, ESocketType socketType) noexcept
    : _log(Log::Get("Network")),
      _context(context),
      _socketType(socketType)
{
}

Context &Network::GetContext() noexcept
{
	return _context;
}

void Network::Initialize() noexcept
{
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
}

void Network::Deinitialize() noexcept
{
	_clients.clear();
	_servers.clear();
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

void Network::ChangeClientSocket(ESocketType socketType, std::int32_t uid)
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

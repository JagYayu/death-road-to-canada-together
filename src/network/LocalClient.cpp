#include "network/LocalClient.hpp"

#include "network/LocalServer.hpp"
#include "network/NetworkManager.hpp"
#include "network/SocketType.hpp"

#include <memory>

using namespace tudov;

LocalClient::LocalClient(INetworkManager &network) noexcept
    : _networkManager(network)
{
}

INetworkManager &LocalClient::GetNetworkManager() noexcept
{
	return _networkManager;
}

ESocketType LocalClient::GetSocketType() const noexcept
{
	return ESocketType::Local;
}

bool LocalClient::IsConnecting() noexcept
{
	return false;
}

bool LocalClient::IsConnected() noexcept
{
	return _connected;
}

void LocalClient::Connect(const IClient::ConnectArgs &baseArgs)
{
	const ConnectArgs &args = dynamic_cast<const ConnectArgs &>(baseArgs);

	LocalServer *localServer = args.server;
	if (localServer == nullptr)
	{
		if (auto &&server = GetNetworkManager().GetServer(); server != nullptr)
		{
			localServer = dynamic_cast<LocalServer *>(server);
		}
	}

	localServer->AddClient(args.user, shared_from_this());
	_connected = true;
}

void LocalClient::Disconnect()
{
	_connected = false;
}

void LocalClient::SendReliable(std::string_view data)
{
}

void LocalClient::SendUnreliable(std::string_view data)
{
}

bool LocalClient::Update()
{
	return false; // TODO
}

void LocalClient::Receive(std::string_view data) noexcept
{
	_messageQueue.emplace(std::string(data));
}

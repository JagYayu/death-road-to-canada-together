#include "LocalClient.hpp"

#include "LocalServer.hpp"
#include "Network.hpp"
#include "SocketType.hpp"
#include <memory>

using namespace tudov;

LocalClient::LocalClient(INetwork &network) noexcept
    : _network(network)
{
}

INetwork &LocalClient::GetNetwork() noexcept
{
	return _network;
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
		if (auto &&server = GetNetwork().GetServer(); server != nullptr)
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

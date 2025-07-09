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
		if (auto &&server = GetNetwork().GetServer(); !server.expired())
		{
			localServer = std::dynamic_pointer_cast<LocalServer>(server.lock()).get();
		}
	}

	localServer->AddClient(args.user, shared_from_this());
	_connected = true;
}

void LocalClient::Disconnect()
{
	_connected = false;
}

bool LocalClient::Update()
{
	return false; // TODO
}

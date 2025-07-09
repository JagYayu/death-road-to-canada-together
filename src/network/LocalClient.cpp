#include "LocalClient.hpp"

#include "SocketType.hpp"

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

void LocalClient::Connect(const ConnectArgs &args)
{
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

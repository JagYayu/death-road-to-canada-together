#include "Network.hpp"

#include "LocalClient.hpp"
#include "LocalServer.hpp"

#include <memory>

using namespace tudov;

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
	_client = std::make_shared<LocalClient>(*this);
	_server = std::make_shared<LocalServer>(*this);
}

void Network::Deinitialize() noexcept
{
}

IClient &Network::GetClient() noexcept
{
	return *_client;
}

IServer &Network::GetServer() noexcept
{
	return *_server;
}

void Network::ChangeSocket(ESocketType socketType)
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

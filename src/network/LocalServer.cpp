#include "LocalServer.hpp"

#include "SocketType.hpp"
#include <optional>

using namespace tudov;

LocalServer::LocalServer(INetwork &network) noexcept
    : _network(network)
{
}

INetwork &LocalServer::GetNetwork() noexcept
{
	return _network;
}

ESocketType LocalServer::GetSocketType() const noexcept
{
	return _socketType;
}

std::optional<std::string_view> LocalServer::GetTitle() noexcept
{
	return _hostInfo != nullptr ? std::make_optional(_hostInfo->title) : std::nullopt;
}

std::optional<std::string_view> LocalServer::GetPassword() noexcept
{
	return _hostInfo != nullptr ? std::make_optional(_hostInfo->password) : std::nullopt;
}

std::optional<std::int32_t> LocalServer::GetMaxClients() noexcept
{
	return _hostInfo != nullptr ? std::make_optional(_hostInfo->maximumClients) : std::nullopt;
}

bool LocalServer::IsHosting() noexcept
{
	return _hostInfo != nullptr;
}

void LocalServer::Host(const HostArgs &args)
{
	_hostInfo = std::make_unique<HostInfo>(HostInfo{
	    .title = args.title,
	    .password = args.password,
	    .maximumClients = args.maximumClients,
	});
}

void LocalServer::Shutdown()
{
	_hostInfo = nullptr;
}

bool LocalServer::Update()
{
	return false;
}

void LocalServer::AddClient(std::uint32_t uid, const std::weak_ptr<LocalClient> &localClient)
{
	
	// TODO
}

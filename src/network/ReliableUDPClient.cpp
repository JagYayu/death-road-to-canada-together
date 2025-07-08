#include "ReliableUDPClient.h"

#include "enet/enet.h"

using namespace tudov;

static std::uint32_t enetRefCount = 0;

ReliableUDPClient::ReliableUDPClient(Context &context) noexcept
    : _context(context)
{
	if (enetRefCount == 0)
	{
		enet_initialize();
	}
	++enetRefCount;
}

ReliableUDPClient::~ReliableUDPClient() noexcept
{
	--enetRefCount;
	if (enetRefCount == 0)
	{
		enet_deinitialize();
	}
}

Context &ReliableUDPClient::GetContext() noexcept
{
	return _context;
}

ESocketType ReliableUDPClient::GetSocketType() const noexcept
{
	return ESocketType::ReliableUDP;
}

void ReliableUDPClient::Connect(std::string_view address)
{
	// ENetAddress
	// _eNetHost=enet_host_create(address.)
}

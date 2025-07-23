#include "network/NetworkComponent.hpp"
#include "network/NetworkManager.hpp"

using namespace tudov;

Context &INetworkComponent::GetContext() noexcept
{
	return GetNetworkManager().GetContext();
}

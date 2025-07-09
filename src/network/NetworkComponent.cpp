#include "NetworkComponent.hpp"

#include "Network.hpp"

using namespace tudov;

Context &INetworkComponent::GetContext() noexcept
{
	return GetNetwork().GetContext();
}

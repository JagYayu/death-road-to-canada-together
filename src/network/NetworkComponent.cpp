/**
 * @file network/NetworkComponent.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "network/NetworkComponent.hpp"

#include "network/NetworkManager.hpp"

using namespace tudov;

Context &INetworkComponent::GetContext() noexcept
{
	return GetNetworkManager().GetContext();
}

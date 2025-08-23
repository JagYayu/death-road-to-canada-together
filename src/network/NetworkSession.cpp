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

#include "network/NetworkSession.hpp"

#include "network/NetworkManager.hpp"

using namespace tudov;

Context &INetworkSession::GetContext() noexcept
{
	return GetNetworkManager().GetContext();
}

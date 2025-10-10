/**
 * @file network/ReliableUDPSession.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Network/ReliableUDPSession.hpp"

#include "enet/enet.h"

#include <cstdint>

using namespace tudov;

static std::uint32_t enetRefCount = 0;

void ReliableUDPSession::OnENetSessionInitialize()
{
	if (enetRefCount == 0)
	{
		enet_initialize();
	}
	++enetRefCount;
}

void ReliableUDPSession::OnENetSessionDeinitialize()
{
	--enetRefCount;
	if (enetRefCount == 0)
	{
		enet_deinitialize();
	}
}

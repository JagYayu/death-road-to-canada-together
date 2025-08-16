/**
 * @file event/CoreEventsData.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "event/CoreEventsData.hpp"

using namespace tudov;

EventReliableUDPConnectData::EventReliableUDPConnectData(_ENetPeer *eNetPeer) noexcept
    : eNetPeer(eNetPeer)
{
}

#include "event/CoreEventsData.hpp"

using namespace tudov;

EventReliableUDPConnectData::EventReliableUDPConnectData(_ENetPeer *eNetPeer) noexcept
    : eNetPeer(eNetPeer)
{
}

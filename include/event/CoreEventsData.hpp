#pragma once

struct _ENetPeer;

namespace tudov
{
	struct CoreEventData
	{
		virtual ~CoreEventData() noexcept = default;
	};

	struct EventReliableUDPConnectData : public CoreEventData
	{
		_ENetPeer *eNetPeer;

		explicit EventReliableUDPConnectData(_ENetPeer *eNetPeer) noexcept;
		~EventReliableUDPConnectData() noexcept override = default;
	};
} // namespace tudov

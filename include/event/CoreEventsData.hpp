/**
 * @file event/CoreEventsData.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

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

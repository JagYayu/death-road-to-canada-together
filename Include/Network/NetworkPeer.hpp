/**
 * @file network/NetworkPeer.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <cstdint>

struct _ENetPeer;

namespace tudov
{
	class NetworkPeer
	{
	  private:
		_ENetPeer *_eNetPeer;

	  public:
		explicit NetworkPeer(_ENetPeer *eNetPeer) noexcept;
		explicit NetworkPeer(const NetworkPeer &) noexcept = default;
		explicit NetworkPeer(NetworkPeer &&) noexcept = default;
		NetworkPeer &operator=(const NetworkPeer &) noexcept = default;
		NetworkPeer &operator=(NetworkPeer &&) noexcept = default;
		~NetworkPeer() noexcept = default;

		std::uint32_t GetConnectID() const noexcept;
	};
} // namespace tudov

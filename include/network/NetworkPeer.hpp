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

		std::uint32_t GetConnectID() const noexcept;
	};
} // namespace tudov

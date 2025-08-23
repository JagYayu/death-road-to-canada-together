/**
 * @file network/ClientSessionState.hpp
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

namespace tudov
{
	enum class EClientSessionState : std::uint8_t
	{
		Disconnected,
		Connecting,
		Connected,
		Disconnecting,
	};
} // namespace tudov

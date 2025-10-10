/**
 * @file network/ReliableUDPSession.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Util/Micros.hpp"

namespace tudov
{
	struct ReliableUDPSession
	{
		TE_STATIC_CLASS(ReliableUDPSession);

		static void OnENetSessionInitialize();
		static void OnENetSessionDeinitialize();
	};
} // namespace tudov

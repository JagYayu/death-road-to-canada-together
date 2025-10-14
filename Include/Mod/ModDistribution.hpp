/**
 * @file Mod/ModDistribution.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "json.hpp"

#include <cstdint>

namespace tudov
{
	enum class EModDistribution : uint8_t
	{
		ClientOnly = 1 << 0,
		ServerOnly = 1 << 1,
		Universal = ClientOnly | ServerOnly,
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(EModDistribution, {
	                                                   {EModDistribution::ClientOnly, "ClientOnly"},
	                                                   {EModDistribution::ClientOnly, "ServerOnly"},
	                                                   {EModDistribution::Universal, "Universal"},
	                                               });
} // namespace tudov
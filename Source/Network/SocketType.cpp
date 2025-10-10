/**
 * @file network/SocketType.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Network/SocketType.hpp"

using namespace tudov;

std::optional<ESocketType> tudov::StringToSocketType(std::string_view str) noexcept
{
	if (str == "Local")
		return std::make_optional(ESocketType::Local);
	if (str == "RUDP")
		return std::make_optional(ESocketType::RUDP);
	if (str == "Steam")
		return std::make_optional(ESocketType::Steam);
	return std::nullopt;
}

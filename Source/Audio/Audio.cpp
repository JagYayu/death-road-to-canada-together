/**
 * @file audio/Audio.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "audio/Audio.hpp"

using namespace tudov;

Audio::Audio(std::string_view path, const std::vector<std::byte> &bytes) noexcept
	: _bytes(bytes.begin(), bytes.end())
{
}

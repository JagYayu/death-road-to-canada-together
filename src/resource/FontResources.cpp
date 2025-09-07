/**
 * @file resource/FontResources.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "resource/FontResources.hpp"

using namespace tudov;

FontResources::FontResources() noexcept
{
}

FontID FontResources::LuaGetID(std::string_view font) noexcept
{
	return GetResourceID(font);
}

std::string_view FontResources::LuaGetPath(FontID id) noexcept
{
	return GetResourcePath(id);
}

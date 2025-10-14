/**
 * @file Mod/Mod.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Mod/ModConfig.hpp"

using namespace tudov;

Version ModConfig::LuaGetVersion() const noexcept
{
	return Version(version);
}

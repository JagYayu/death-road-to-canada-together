/**
 * @file Mod/ModIO.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Mod/ModIO.hpp"

using namespace tudov;

ModIO::ModIO() noexcept
{
}

constexpr EModPortal ModIO::GetModPortalType() const noexcept
{
	return EModPortal::ModIO;
}

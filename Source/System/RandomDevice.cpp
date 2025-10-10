/**
 * @file util/RandomDevice.cpp
 * @author JagYayu
 * @brief Micros for project header files.
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "System/RandomDevice.hpp"

#include "sol/sol.hpp"

#include <random>

using namespace tudov;

static std::random_device randomDevice;

std::double_t RandomDevice::Entropy() noexcept
{
	return randomDevice.entropy();
}

std::double_t RandomDevice::Generate(std::double_t min, std::double_t max) noexcept
{
	return std::uniform_real_distribution<std::double_t>(min, max)(randomDevice);
}

std::double_t RandomDevice::LuaGenerate(sol::object min, sol::object max) noexcept
{
	return Generate(min.as<std::double_t>(), max.as<std::double_t>());
}

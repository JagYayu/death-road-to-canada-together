/**
 * @file system/Time.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "system/Time.hpp"

#include <chrono>

using namespace tudov;

static decltype(auto) startupTime = std::chrono::duration<std::double_t>(std::chrono::system_clock::now().time_since_epoch()).count();

std::double_t Time::GetStartupTime() noexcept
{
	return startupTime;
}

std::double_t Time::GetSystemTime() noexcept
{
	return std::chrono::duration<std::double_t>(std::chrono::system_clock::now().time_since_epoch()).count();
}

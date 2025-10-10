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

#include "System/Time.hpp"

#include <chrono>

using namespace tudov;

static decltype(auto) startupTime = std::chrono::duration<std::double_t>(std::chrono::system_clock::now().time_since_epoch()).count();

Timer::Timer() noexcept
    : Timer(false)
{
}

Timer::Timer(bool paused) noexcept
    : _startTime(std::chrono::steady_clock::now()),
      _paused(paused),
      _pausedTime(std::chrono::steady_clock::now()),
      _pausedElapsed(0)
{
}

void Timer::Pause() noexcept
{
	if (!_paused)
	{
		_pausedTime = std::chrono::steady_clock::now();
		_paused = true;
	}
}

void Timer::Unpause() noexcept
{
	if (_paused)
	{
		_pausedElapsed += (std::chrono::steady_clock::now() - _pausedTime);
		_paused = false;
	}
}

void Timer::Reset() noexcept
{
	_startTime = std::chrono::steady_clock::now();
	_pausedElapsed = std::chrono::duration<double>(0.0);
	_paused = false;
}

std::double_t Timer::GetTime() noexcept
{
	if (_paused)
	{
		return std::chrono::duration<std::double_t>(_pausedTime - _startTime - _pausedElapsed).count();
	}
	else
	{
		return std::chrono::duration<std::double_t>(std::chrono::steady_clock::now() - _startTime - _pausedElapsed).count();
	}
}

std::double_t Time::GetStartupTime() noexcept
{
	return startupTime;
}

std::double_t Time::GetSystemTime() noexcept
{
	return std::chrono::duration<std::double_t>(std::chrono::system_clock::now().time_since_epoch()).count();
}

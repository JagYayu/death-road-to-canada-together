/**
 * @file system/Time.hpp
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

#include <chrono>
#include <cmath>

namespace tudov
{
	class LuaBindings;

	class Timer
	{
	  private:
		std::chrono::steady_clock::time_point _startTime;
		bool _paused;
		std::chrono::steady_clock::time_point _pausedTime;
		std::chrono::duration<std::double_t> _pausedElapsed;

	  public:
		explicit Timer() noexcept;
		explicit Timer(const Timer &) noexcept = default;
		explicit Timer(Timer &&) noexcept = default;
		Timer &operator=(const Timer &) noexcept = default;
		Timer &operator=(Timer &&) noexcept = default;

		explicit Timer(bool paused) noexcept;

		void Pause() noexcept;

		void Unpause() noexcept;

		void Reset() noexcept;

		std::double_t GetTime() noexcept;
	};

	/**
	 * Wrapper for timer classes. Mainly designed for lua api.
	 */
	struct Time
	{
		friend LuaBindings;

		TE_STATIC_CLASS(Time);

		static std::double_t GetStartupTime() noexcept;

		static std::double_t GetSystemTime() noexcept;
	};
} // namespace tudov

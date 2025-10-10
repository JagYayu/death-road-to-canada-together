/**
 * @file debug/DebugUtils.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "util/Definitions.hpp"
#include <cmath>
#include <sstream>
#include <string>

namespace tudov
{
	struct IWindow;

	struct DebugUtils
	{
		explicit DebugUtils() noexcept = delete;
		explicit DebugUtils(const DebugUtils &) noexcept = delete;
		explicit DebugUtils(DebugUtils &&) noexcept = delete;
		DebugUtils &operator=(const DebugUtils &) noexcept = delete;
		DebugUtils &operator=(DebugUtils &&) noexcept = delete;
		~DebugUtils() noexcept = delete;

		template <std::size_t Parts>
		inline static std::array<std::string, Parts> Split(std::string_view arg) noexcept
		{
			std::array<std::string, Parts> result;

			std::istringstream iss{arg.data()};
			std::string id;
			std::size_t count = 0;

			while (iss >> id && count < Parts)
			{
				result[count] = std::move(id);
				++count;
			}

			return result;
		}

		[[deprecated("Use `IWindow::GetGUIScale` instead")]]
		static std::float_t GetWindowGUIScale(IWindow &window) noexcept;
	};
} // namespace tudov

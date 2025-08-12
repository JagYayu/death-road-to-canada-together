#pragma once

#include <cmath>
#include <sstream>
#include <string>

namespace tudov
{
	struct IWindow;

	struct DebugUtils
	{
		explicit DebugUtils() noexcept = delete;
		~DebugUtils() noexcept = delete;

		template <std::size_t Parts>
		inline static std::array<std::string, Parts> Split(std::string_view arg) noexcept
		{
			std::array<std::string, Parts> result;

			std::istringstream iss{arg.data()};
			std::string token;
			std::size_t count = 0;

			while (iss >> token && count < Parts)
			{
				result[count] = std::move(token);
				++count;
			}

			return result;
		}

		[[deprecated("Use `IWindow::GetGUIScale` instead")]]
		static std::float_t GetWindowGUIScale(IWindow &window) noexcept;
	};
} // namespace tudov

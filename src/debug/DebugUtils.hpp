#pragma once

#include <sstream>
#include <string>

namespace tudov
{
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
	};
} // namespace tudov

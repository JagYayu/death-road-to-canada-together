#pragma once

#include "Resource.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace tudov
{
	class TextResource : public IResource
	{
	  private:
		std::string _str;

	  public:
		explicit TextResource() noexcept = default;
		~TextResource() noexcept override = default;

		explicit TextResource(std::vector<std::byte> bytes) noexcept
		{
			std::string_view view{reinterpret_cast<const char *>(bytes.data()), bytes.size()};
			_str = std::string(view);
		}

		explicit TextResource(std::string_view str) noexcept
		    : _str(str)
		{
		}

		operator std::string_view() noexcept
		{
			return _str;
		}

		inline std::string_view View() noexcept
		{
			return _str;
		}
	};
} // namespace tudov

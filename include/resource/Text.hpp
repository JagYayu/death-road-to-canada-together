#pragma once

#include "Resource.hpp"

#include <string>
#include <string_view>
#include <utility>

namespace tudov
{
	class Text : public IResource
	{
	  private:
		std::string str;

	  public:
		explicit Text() noexcept = default;
		~Text() noexcept override = default;

		template <typename... TArgs>
		inline explicit Text(TArgs &&...args) noexcept
		    : str(std::forward<TArgs>(args)...)
		{
		}

		operator std::string_view() noexcept
		{
			return str;
		}

		inline std::string_view View() noexcept
		{
			return str;
		}
	};
} // namespace tudov

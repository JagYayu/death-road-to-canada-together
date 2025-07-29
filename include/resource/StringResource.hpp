#pragma once

#include "Resource.hpp"

#include <string>
#include <string_view>
#include <utility>

namespace tudov
{
	class StringResource : public IResource
	{
	  private:
		std::string _str;

	  public:
		explicit StringResource() noexcept = default;
		~StringResource() noexcept override = default;

		template <typename... TArgs>
		inline explicit StringResource(TArgs &&...args) noexcept
		    : _str(std::forward<TArgs>(args)...)
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

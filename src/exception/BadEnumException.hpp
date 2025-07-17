#pragma once

#include "Exception.hpp"

#include "util/Utils.hpp"

#include <format>

namespace tudov
{
	template <ScopeEnum TEnum>
	class BadEnumException : public Exception
	{
	  protected:
		TEnum _value;

	  public:
		explicit BadEnumException(Context &context, TEnum value) noexcept
		    : Exception(context),
		      _value(value)
		{
			_what = std::format("Bad enumeration {}", EnumToPrettyString(_value));
		}

		inline TEnum GetValue() const noexcept
		{
			return _value;
		}
	};
} // namespace tudov

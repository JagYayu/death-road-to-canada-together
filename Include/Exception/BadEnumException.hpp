/**
 * @file event/BadEnumException.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Exception.hpp"

#include "Util/Utils.hpp"

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
		explicit BadEnumException(const BadEnumException &) noexcept = default;
		explicit BadEnumException(BadEnumException &&) noexcept = default;
		BadEnumException &operator=(const BadEnumException &) noexcept = default;
		BadEnumException &operator=(BadEnumException &&) noexcept = default;
		~BadEnumException() noexcept = default;

		inline TEnum GetValue() const noexcept
		{
			return _value;
		}
	};
} // namespace tudov

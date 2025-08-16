/**
 * @file event/Exception.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "program/Context.hpp"

#include <exception>
#include <string>
#include <string_view>

namespace tudov
{
	class Exception : public std::exception, public IContextProvider
	{
		static constexpr decltype(auto) Default = "Tudov engine exception";

	  protected:
		Context &_context;
		std::string _what;

	  public:
		explicit Exception(Context &context) noexcept;
		explicit Exception(Context &context, std::string_view what) noexcept;
		~Exception() noexcept override = default;

		Context &GetContext() noexcept override;
		const char *what() const noexcept override;

		std::string &What() noexcept;

		inline const std::string &What() const noexcept
		{
			return const_cast<Exception *>(this)->What();
		}
	};
} // namespace tudov

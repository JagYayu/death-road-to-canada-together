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

#include "Program/Context.hpp"

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
		mutable std::string _what;

	  public:
		explicit Exception(Context &context) noexcept;
		explicit Exception(Context &context, std::string_view what) noexcept;
		explicit Exception(const Exception &) noexcept = default;
		explicit Exception(Exception &&) noexcept = default;
		Exception &operator=(const Exception &) noexcept = delete;
		Exception &operator=(Exception &&) noexcept = delete;
		~Exception() noexcept override = default;

		Context &GetContext() noexcept override;

		const char *what() const noexcept override;

		virtual std::string_view What() const noexcept
		{
			return _what;
		}
	};
} // namespace tudov

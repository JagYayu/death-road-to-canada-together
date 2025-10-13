/**
 * @file program/Tudov.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Util/Micros.hpp"

#include <format>
#include <memory>
#include <source_location>
#include <string_view>

namespace tudov
{
	struct Application;
	struct MainArgs;
	class Config;

	struct Tudov
	{
		TE_STATIC_CLASS(Tudov);

		/**
		 * Abort the application with fatal error.
		 */
		[[noreturn]] static void FatalError(std::string_view errorMessage) noexcept;

		/**
		 * Get application.
		 * Must be called once at the begining of lifespan, to initialize the application.
		 */
		static std::shared_ptr<Application> GetApplication() noexcept;

		/**
		 * Release application.
		 * Must be called at the end of lifespan.
		 */
		static void ReleaseApplication() noexcept;

		/**
		 * Get application main args.
		 */
		static const MainArgs &GetMainArgs() noexcept;

		/**
		 * Initialize application main args.
		 * Must be called at the begining of lifespan.
		 */
		static void InitMainArgs(int argc, char **argv);

		/**
		 * Get application config.
		 */
		static Config &GetConfig() noexcept;

		/**
		 * Initialize application config.
		 * Must be called at the begining of lifespan.
		 */
		static void InitConfig() noexcept;

		/**
		 * @see `GetApplication`.
		 */
		template <typename T>
		static std::shared_ptr<T> GetApplication() noexcept
		{
			return std::dynamic_pointer_cast<T>(GetApplication());
		}
	};
} // namespace tudov

namespace tudov::impl
{
	TE_FORCEINLINE void AssertFailed(std::string_view condition, const std::source_location &location, void * = nullptr) noexcept
	{
		auto message = std::format("Assertion failed: {}\nFile: {}\nLine: {}\nFunction: {}",
		                           condition,
		                           location.file_name(),
		                           location.line(),
		                           location.function_name());
		Tudov::FatalError(message);
	}

	template <typename... TArgs>
	TE_FORCEINLINE void AssertFailed(std::string_view condition, const std::source_location &location, std::format_string<TArgs...> fmt, TArgs &&...args) noexcept
	{
		auto message = std::format("Assertion failed: {}\nFile: {}\nLine: {}\nFunction: {}\nMessage: {}",
		                           condition,
		                           location.file_name(),
		                           location.line(),
		                           location.function_name(),
		                           std::format(fmt, std::forward<TArgs>(args)...));
		Tudov::FatalError(message);
	}
} // namespace tudov::impl

#define TE_ASSERT(Condition, ...)                                                   \
	do                                                                              \
	{                                                                               \
		if (!(Condition)) [[unlikely]]                                              \
		{                                                                           \
			auto src = ::std::source_location::current();                           \
			::tudov::impl::AssertFailed(#Condition, src __VA_OPT__(, __VA_ARGS__)); \
		}                                                                           \
	} while (false)

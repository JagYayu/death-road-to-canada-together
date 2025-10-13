/**
 * @file util/SystemUtils.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Micros.hpp"

#include <cstdint>
#include <exception>
#include <filesystem>
#include <functional>
#include <string_view>
#include <thread>

namespace tudov
{
	struct SystemUtils
	{
		TE_STATIC_CLASS(SystemUtils);

		using SuccessHandle = std::function<void(const std::filesystem::path &file, std::uint32_t line)>;
		using ExceptionHandle = std::function<void(const std::filesystem::path &file, std::uint32_t line, std::exception &e)>;

		// Clipboard

		static bool IsClipboardAvailable() noexcept;
		static void CopyToClipboard(std::string_view text) noexcept;
		static void ClearClipboard() noexcept;

		// Script Editor

		static bool IsScriptEditorAvailable() noexcept;
		static void OpenScriptEditor(std::filesystem::path file, std::uint32_t line);
		static std::thread OpenScriptEditorAsync(const std::filesystem::path &file, std::uint32_t line, const SuccessHandle &successHandle = _defaultSuccessHandle, const ExceptionHandle &exceptionHandle = _defaultExceptionHandle) noexcept;

	  private:
		static SuccessHandle _defaultSuccessHandle;
		static ExceptionHandle _defaultExceptionHandle;
	};
} // namespace tudov

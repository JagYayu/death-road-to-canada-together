/**
 * @file util/SystemUtils.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "util/SystemUtils.hpp"

#include "system/LogMicros.hpp"

using namespace tudov;

SystemUtils::SuccessHandle SystemUtils::_defaultSuccessHandle = [](const std::filesystem::path &file, std::uint32_t line) {};

SystemUtils::ExceptionHandle SystemUtils::_defaultExceptionHandle = [](const std::filesystem::path &file, std::uint32_t line, std::exception &e)
{
	TE_G_ERROR(TE_NAMEOF(SystemUtils), "{}", e.what());
};

std::thread SystemUtils::OpenScriptEditorAsync(const std::filesystem::path &file, std::uint32_t line, const SuccessHandle &successHandle, const ExceptionHandle &exceptionHandle) noexcept
{
	auto thread = std::thread([file, line, successHandle, exceptionHandle]()
	{
		try
		{
			OpenScriptEditor(file, line);
		}
		catch (std::exception &e)
		{
			exceptionHandle(file, line, e);
		}
		catch (...)
		{
			std::exception_ptr ptr = std::current_exception();
			try
			{
				if (ptr)
				{
					std::rethrow_exception(ptr);
				}
			}
			catch (std::exception &e)
			{
				exceptionHandle(file, line, e);
			}
		}

		successHandle(file, line);
	});

	thread.detach();

	return thread;
}

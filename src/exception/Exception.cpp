/**
 * @file event/Exception.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "exception/Exception.hpp"

using namespace tudov;

Exception::Exception(Context &context) noexcept
    : _context(context),
      std::exception(Default)
{
}

Exception::Exception(Context &context, std::string_view what) noexcept
    : _context(context),
      std::exception(Default)
{
}

const char *Exception::what() const noexcept
{
	return What().data();
}

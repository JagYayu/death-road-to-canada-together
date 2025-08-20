/**
 * @file misc/Text.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "misc/Text.hpp"

#include "system/Log.hpp"

using namespace tudov;

Text::Text(std::string_view path, const std::vector<std::byte> &bytes) noexcept
    : _path(path)
{
	std::string_view view{reinterpret_cast<const char *>(bytes.data()), bytes.size()};
	_view = view;
}

std::string_view Text::GetFilePath() const noexcept
{
	return _path;
}

Text::operator std::string_view() const noexcept
{
	return _view;
}

std::string_view Text::View() const noexcept
{
	return _view;
}

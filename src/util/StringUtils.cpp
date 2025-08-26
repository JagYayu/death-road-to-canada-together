/**
 * @file util/StringUtils.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "util/StringUtils.hpp"

#include <string>

using namespace tudov;

std::string StringUtils::FilePathToLuaScriptName(std::string_view filePath) noexcept
{
	auto result = std::string(filePath);

	std::replace(result.begin(), result.end(), '/', '.');
	std::replace(result.begin(), result.end(), '\\', '.');

	std::size_t lastDotPos = result.find_last_of('.');
	if (lastDotPos != std::string::npos && lastDotPos > 0)
	{
		std::size_t lastSlashPos = result.find_last_of("/\\");
		if (lastSlashPos == std::string::npos || lastDotPos > lastSlashPos)
		{
			result.erase(lastDotPos);
		}
	}

	return result;
}

bool StringUtils::IsSubpath(const std::filesystem::path &parent, const std::filesystem::path &child) noexcept
{
	auto p = std::filesystem::weakly_canonical(parent).lexically_normal();
	auto c = std::filesystem::weakly_canonical(child).lexically_normal();

	auto pIt = p.begin();
	auto cIt = c.begin();
	for (; pIt != p.end() && cIt != c.end(); ++pIt, ++cIt)
	{
		if (*pIt != *cIt)
		{
			return false;
		}
	}

	return pIt == p.end();
}

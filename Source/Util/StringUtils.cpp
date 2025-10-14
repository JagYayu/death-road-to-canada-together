/**
 * @file Util/StringUtils.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Util/StringUtils.hpp"

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

std::string StringUtils::Unescape(std::string_view s) noexcept
{
	std::string result;
	result.reserve(s.size());

	for (std::size_t i = 0; i < s.size(); ++i)
	{
		if (s[i] == '\\' && i + 1 < s.size())
		{
			switch (s[i + 1])
			{
			case 'n':
				result.push_back('\n');
				++i;
				break;
			case 'r':
				result.push_back('\r');
				++i;
				break;
			case 't':
				result.push_back('\t');
				++i;
				break;
			case '\\':
				result.push_back('\\');
				++i;
				break;
			case '"':
				result.push_back('"');
				++i;
				break;
			default:
				result.push_back(s[i]);
				break;
			}
		}
		else
		{
			result.push_back(s[i]);
		}
	}

	return result;
}

#pragma once

#include <algorithm>
#include <string>

namespace tudov
{
	inline std::string toLuaRequirePath(const std::string &filepath)
	{
		std::string result = filepath;

		std::replace(result.begin(), result.end(), '/', '.');
		std::replace(result.begin(), result.end(), '\\', '.');

		const std::string suffix = ".lua";
		if (result.size() >= suffix.size() && result.compare(result.size() - suffix.size(), suffix.size(), suffix) == 0)
		{
			result.erase(result.size() - suffix.size());
		}

		return result;
	}
} // namespace tudov
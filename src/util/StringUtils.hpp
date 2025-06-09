#pragma once

#include <algorithm>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>

namespace tudov
{
	struct StringTransHash
	{
		using is_transparent = void;

		size_t operator()(const std::string &str) const noexcept
		{
			return std::hash<std::string>{}(str);
		}

		size_t operator()(const std::string_view &str) const noexcept
		{
			return std::hash<std::string_view>{}(str);
		}
	};

	struct StringTransEqual
	{
		using is_transparent = void;

		bool operator()(const std::string &lhs, const std::string &rhs) const noexcept
		{
			return lhs == rhs;
		}

		bool operator()(const std::string_view &lhs, const std::string &rhs) const noexcept
		{
			return lhs == rhs;
		}

		bool operator()(const std::string &lhs, const std::string_view &rhs) const noexcept
		{
			return lhs == rhs;
		}

		bool operator()(const std::string_view &lhs, const std::string_view &rhs) const noexcept
		{
			return lhs == rhs;
		}
	};

	inline static const std::string emptyString = "";
	inline static const std::string luaFileExtension = ".lua";

	inline std::string GetLuaNamespace(const std::string scriptName)
	{
		size_t pos = scriptName.find('.');
		if (pos == std::string::npos)
		{
			return emptyString;
		}
		return scriptName.substr(0, pos);
	}

	inline std::string ReadFileToString(const std::string &file)
	{
		std::ifstream stream{file};
		if (!stream)
		{
			throw std::runtime_error("Failed to open file: " + file);
		}
		return std::string{std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
	}

	inline std::string FilePathToLuaScriptName(const std::string &filePath)
	{
		std::string result = filePath;

		std::replace(result.begin(), result.end(), '/', '.');
		std::replace(result.begin(), result.end(), '\\', '.');

		if (result.size() >= luaFileExtension.size() && result.compare(result.size() - luaFileExtension.size(), luaFileExtension.size(), luaFileExtension) == 0)
		{
			result.erase(result.size() - luaFileExtension.size());
		}

		return result;
	}
} // namespace tudov
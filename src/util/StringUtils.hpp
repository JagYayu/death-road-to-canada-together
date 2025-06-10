#pragma once

#include "util/Defs.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <stdexcept>

namespace tudov
{
	struct StringSVHash
	{
		using is_transparent = void;

		size_t operator()(const String &str) const noexcept
		{
			return std::hash<String>{}(str);
		}

		size_t operator()(const StringView &str) const noexcept
		{
			return std::hash<StringView>{}(str);
		}
	};

	struct StringSVEqual
	{
		using is_transparent = void;

		bool operator()(const String &lhs, const String &rhs) const noexcept
		{
			return lhs == rhs;
		}

		bool operator()(const StringView &lhs, const String &rhs) const noexcept
		{
			return lhs == rhs;
		}

		bool operator()(const String &lhs, const StringView &rhs) const noexcept
		{
			return lhs == rhs;
		}

		bool operator()(const StringView &lhs, const StringView &rhs) const noexcept
		{
			return lhs == rhs;
		}
	};

	inline static const String emptyString = "";
	inline static const String luaFileExtension = ".lua";

	inline String GetLuaNamespace(const String &scriptName)
	{
		size_t pos = scriptName.find('.');
		if (pos == String::npos)
		{
			return emptyString;
		}
		return scriptName.substr(0, pos);
	}

	inline String GetLuaNamespace(const StringView &scriptName)
	{
		size_t pos = scriptName.find('.');
		if (pos == String::npos)
		{
			return emptyString;
		}
		return String(scriptName).substr(0, pos);
	}

	inline String ReadFileToString(const String &file)
	{
		std::ifstream stream{file};
		if (!stream)
		{
			throw std::runtime_error("Failed to open file: " + file);
		}
		return String{std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
	}

	inline String FilePathToLuaScriptName(const String &filePath)
	{
		String result = filePath;

		std::replace(result.begin(), result.end(), '/', '.');
		std::replace(result.begin(), result.end(), '\\', '.');

		if (result.size() >= luaFileExtension.size() && result.compare(result.size() - luaFileExtension.size(), luaFileExtension.size(), luaFileExtension) == 0)
		{
			result.erase(result.size() - luaFileExtension.size());
		}

		return result;
	}
} // namespace tudov
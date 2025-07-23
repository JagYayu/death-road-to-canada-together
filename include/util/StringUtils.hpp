#pragma once

#include <algorithm>
#include <fstream>
#include <iterator>
#include <stdexcept>

namespace tudov
{
	struct StringSVHash
	{
		using is_transparent = void;

		size_t operator()(const std::string &str) const noexcept
		{
			return std::hash<std::string>{}(str);
		}

		size_t operator()(std::string_view str) const noexcept
		{
			return std::hash<std::string_view>{}(str);
		}
	};

	struct StringSVEqual
	{
		using is_transparent = void;

		bool operator()(const std::string &lhs, const std::string &rhs) const noexcept
		{
			return lhs == rhs;
		}

		bool operator()(std::string_view lhs, const std::string &rhs) const noexcept
		{
			return lhs == rhs;
		}

		bool operator()(const std::string &lhs, std::string_view rhs) const noexcept
		{
			return lhs == rhs;
		}

		bool operator()(std::string_view lhs, std::string_view rhs) const noexcept
		{
			return lhs == rhs;
		}
	};

	inline static const std::string emptyString = "";
	inline static const std::string luaFileExtension = ".lua";

	inline std::string GetLuaNamespace(const std::string &scriptName)
	{
		size_t pos = scriptName.find('.');
		if (pos == std::string::npos)
		{
			return emptyString;
		}
		return scriptName.substr(0, pos);
	}

	inline std::string GetLuaNamespace(std::string_view scriptName)
	{
		size_t pos = scriptName.find('.');
		if (pos == std::string::npos)
		{
			return emptyString;
		}
		return std::string(scriptName).substr(0, pos);
	}

	inline std::string ReadFileToString(const std::string &file, bool binary = false)
	{
		std::ifstream stream{file, binary ? std::ios::binary : std::ios::in};
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

	inline std::u32string Utf8ToTtf32(const std::string_view &utf8str)
	{
		std::u32string utf32str;
		size_t i = 0;
		while (i < utf8str.size())
		{
			uint32_t ch = 0;
			unsigned char c = utf8str[i];
			size_t extra = 0;

			if (c < 0x80)
			{
				ch = c;
				extra = 0;
			}
			else if ((c >> 5) == 0x6)
			{
				ch = c & 0x1F;
				extra = 1;
			}
			else if ((c >> 4) == 0xE)
			{
				ch = c & 0x0F;
				extra = 2;
			}
			else if ((c >> 3) == 0x1E)
			{
				ch = c & 0x07;
				extra = 3;
			}

			if (i + extra >= utf8str.size())
				break; // invalid utf8

			for (size_t j = 1; j <= extra; ++j)
			{
				ch = (ch << 6) | (utf8str[i + j] & 0x3F);
			}

			utf32str.push_back(ch);
			i += extra + 1;
		}

		return utf32str;
	}

} // namespace tudov
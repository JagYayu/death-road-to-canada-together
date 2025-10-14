/**
 * @file Util/Version.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Util/Version.hpp"

#include <cstdint>
#include <sstream>
#include <stdexcept>

using namespace tudov;

Version::Version() noexcept
    : _parts{0, 0, 0}
{
}

Version::Version(Type major, Type minor, Type patch) noexcept
    : _parts{major, minor, patch}
{
}

Version::Version(std::string_view str) noexcept
{
	std::stringstream ss{str.data()};
	std::string token;
	while (std::getline(ss, token, '.'))
	{
		try
		{
			_parts.emplace_back(std::stoi(token));
		}
		catch (const std::invalid_argument &)
		{
			_parts.emplace_back(0);
		}
	}

	while (_parts.size() < 3)
	{
		_parts.push_back(0);
	}
	_parts.shrink_to_fit();
}

Version::Type &Version::Major()
{
	return _parts[0];
}

Version::Type &Version::Minor()
{
	return _parts[1];
}

Version::Type &Version::Patch()
{
	return _parts[2];
}

Version::Type Version::Major() const
{
	return _parts[0];
}

Version::Type Version::Minor() const
{
	return _parts[1];
}

Version::Type Version::Patch() const
{
	return _parts[2];
}

Version::Type Version::operator[](size_t i) const
{
	return _parts[i];
}

size_t Version::GetSize() const
{
	return _parts.size();
}

std::vector<Version::Type>::const_iterator Version::begin() const
{
	return _parts.begin();
}

std::vector<Version::Type>::const_iterator Version::end() const
{
	return _parts.end();
}

auto Version::operator<=>(const Version &other) const
{
	for (size_t i = 0; i < 3; ++i)
	{
		int lhs = (i < _parts.size()) ? _parts[i] : 0;
		int rhs = (i < other._parts.size()) ? other._parts[i] : 0;
		if (lhs != rhs)
		{
			return lhs <=> rhs;
		}
	}
	return std::strong_ordering::equal;
}

bool Version::operator==(const Version &other) const = default;

std::ostream &tudov::operator<<(std::ostream &os, const Version &v)
{
	os << v.Major() << "." << v.Minor() << "." << v.Patch();
	return os;
}

Version::Type Version::LuaGetMajor() noexcept
{
	return Major();
}

Version::Type Version::LuaGetMinor() noexcept
{
	return Minor();
}

Version::Type Version::LuaGetPatch() noexcept
{
	return Patch();
}

void Version::LuaSetMajor(Type value) noexcept
{
	Major() = value;
}

void Version::LuaSetMinor(Type value) noexcept
{
	Minor() = value;
}

void Version::LuaSetPatch(Type value) noexcept
{
	Patch() = value;
}

std::string Version::LuaToString() noexcept
{
	std::ostringstream oss{};

	for (std::size_t i = 0; i < _parts.size(); ++i)
	{
		if (i > 0) [[likely]]
		{
			oss << '.';
		}
		oss << _parts[i];
	}

	return oss.str();
}

void tudov::from_json(const nlohmann::json &j, Version &v)
{
	v = Version(j.get<std::string>());
}

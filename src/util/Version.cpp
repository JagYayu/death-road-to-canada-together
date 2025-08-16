/**
 * @file util/Version.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "util/Version.hpp"

#include <cstdint>
#include <sstream>

using namespace tudov;

Version::Version()
    : _parts{0, 0, 0}
{
}

Version::Version(int32_t major, int32_t minor, int32_t patch)
    : _parts{major, minor, patch}
{
}

Version::Version(std::string_view str)
{
	std::stringstream ss{str.data()};
	std::string token;
	while (std::getline(ss, token, '.'))
	{
		_parts.push_back(std::stoi(token));
	}
	while (_parts.size() < 3)
	{
		_parts.push_back(0);
	}
	_parts.shrink_to_fit();
}

int32_t Version::Major() const
{
	return _parts[0];
}

int32_t Version::Minor() const
{
	return _parts[1];
}

int32_t Version::Patch() const
{
	return _parts[2];
}

int32_t Version::operator[](size_t i) const
{
	return _parts[i];
}

size_t Version::Size() const
{
	return _parts.size();
}

std::vector<int32_t>::const_iterator Version::begin() const
{
	return _parts.begin();
}

std::vector<int32_t>::const_iterator Version::end() const
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

void tudov::from_json(const nlohmann::json &j, Version &v)
{
	v = Version(j.get<std::string>());
}

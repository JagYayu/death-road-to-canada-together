#pragma once

#include "lib/json.hpp"
#include "util/Defs.h"

#include <compare>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace tudov
{
	class Version
	{
	  private:
		Vector<Int32> _parts;

	  public:
		Version();
		explicit Version(Int32 major, Int32 minor = 0, Int32 patch = 0);
		explicit Version(const String &str);

		Int32 major() const;
		Int32 minor() const;
		Int32 patch() const;

		Int32 operator[](size_t i) const;
		size_t size() const;

		Vector<Int32>::const_iterator begin() const;
		Vector<Int32>::const_iterator end() const;

		auto operator<=>(const Version &other) const;
		bool operator==(const Version &other) const;

		friend std::ostream &operator<<(std::ostream &os, const Version &v);
	};

	void from_json(const nlohmann::json &j, Version &v);
} // namespace tudov

namespace std
{
	template <>
	struct hash<tudov::Version>
	{
		std::size_t operator()(const tudov::Version &v) const
		{
			std::size_t h = 0;
			for (auto &&part : v)
			{
				h ^= std::hash<tudov::Int32>()(part) + 0x9e3779b9 + (h << 6) + (h >> 2);
			}
			return h;
		}
	};
} // namespace std

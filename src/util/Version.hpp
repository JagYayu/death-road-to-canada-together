#pragma once

#include "util/Defs.hpp"

#include <compare>
#include <iostream>
#include <json.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace tudov
{
	class Version
	{
	  private:
		std::vector<int32_t> _parts;

	  public:
		Version();
		explicit Version(int32_t major, int32_t minor = 0, int32_t patch = 0);
		explicit Version(const std::string &str);

		int32_t major() const;
		int32_t minor() const;
		int32_t patch() const;

		int32_t operator[](size_t i) const;
		size_t size() const;

		std::vector<int32_t>::const_iterator begin() const;
		std::vector<int32_t>::const_iterator end() const;

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
				h ^= std::hash<int32_t>()(part) + 0x9e3779b9 + (h << 6) + (h >> 2);
			}
			return h;
		}
	};
} // namespace std

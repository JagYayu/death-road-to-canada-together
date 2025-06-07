#pragma once

#include <json.hpp>

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
		std::vector<int32_t> _parts;

	public:
		Version();
		explicit Version(int32_t major, int32_t minor = 0, int32_t patch = 0);
		explicit Version(const std::string &str);

		int32_t major() const;
		int32_t minor() const;
		int32_t patch() const;

		auto operator<=>(const Version &other) const;
		bool operator==(const Version &other) const;

		friend std::ostream &operator<<(std::ostream &os, const Version &v);
	};

	void from_json(const nlohmann::json &j, Version &v);
}

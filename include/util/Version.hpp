/**
 * @file util/Version.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "json.hpp"

#include <iostream>
#include <vector>

namespace tudov
{
	class LuaAPI;

	class Version
	{
		friend LuaAPI;

	  private:
		std::vector<int32_t> _parts;

	  public:
		static const Version Latest;

	  public:
		Version();
		Version(int32_t major, int32_t minor = 0, int32_t patch = 0);
		Version(std::string_view str);
		~Version() noexcept = default;

	  public:
		int32_t Major() const;
		int32_t Minor() const;
		int32_t Patch() const;

		int32_t operator[](size_t i) const;
		size_t Size() const;

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
		inline std::size_t operator()(const tudov::Version &version) const noexcept
		{
			std::size_t h = 0;
			for (auto &&part : version)
			{
				h ^= std::hash<int32_t>{}(part) + 0x9e3779b9 + (h << 6) + (h >> 2);
			}
			return h;
		}
	};
} // namespace std

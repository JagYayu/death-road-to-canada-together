/**
 * @file Util/Version.hpp
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
	class LuaBindings;

	class Version
	{
		friend LuaBindings;

	  public:
		using Type = std::int32_t;

	  private:
		std::vector<Type> _parts;

	  public:
		static const Version Latest;

	  public:
		Version() noexcept;
		explicit Version(Type major, Type minor = 0, Type patch = 0) noexcept;
		explicit Version(std::string_view str) noexcept;
		Version(const Version &) noexcept = default;
		Version(Version &&) noexcept = default;
		Version &operator=(const Version &) noexcept = default;
		Version &operator=(Version &&) noexcept = default;
		~Version() noexcept = default;

	  public:
		Type &Major();
		Type &Minor();
		Type &Patch();
		Type Major() const;
		Type Minor() const;
		Type Patch() const;

		Type operator[](std::size_t i) const;

		auto operator<=>(const Version &other) const;
		bool operator==(const Version &other) const;

		friend std::ostream &operator<<(std::ostream &os, const Version &v);

		std::size_t GetSize() const;

		std::vector<Type>::const_iterator begin() const;
		std::vector<Type>::const_iterator end() const;

	  private:
		Type LuaGetMajor() noexcept;
		Type LuaGetMinor() noexcept;
		Type LuaGetPatch() noexcept;
		void LuaSetMajor(Type value) noexcept;
		void LuaSetMinor(Type value) noexcept;
		void LuaSetPatch(Type value) noexcept;
		std::string LuaToString() noexcept;
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

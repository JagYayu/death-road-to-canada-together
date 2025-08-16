/**
 * @file mod/ModRequirement.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "util/Version.hpp"

#include <optional>
#include <string>

namespace tudov
{
	struct ModRequirement
	{
	  public:
		std::string uid;
		std::optional<Version> version;
		int32_t priority;

		explicit ModRequirement(std::string_view uid, const std::optional<Version> &version, int32_t priority)
		    : uid(std::string(uid)),
		      version(version),
		      priority(priority)
		{
		}

		bool operator==(const ModRequirement &other) const
		{
			return uid == other.uid && version == other.version;
		}
	};

	// struct ModEntry
	// {

	// };
} // namespace tudov

namespace std
{
	template <>
	struct hash<tudov::ModRequirement>
	{
		size_t operator()(const tudov::ModRequirement &m) const
		{
			size_t h1 = hash<string>()(m.uid);
			size_t h2 = m.version ? hash<tudov::Version>()(*m.version) : 0;
			size_t h3 = hash<int32_t>()(m.priority);
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};
} // namespace std

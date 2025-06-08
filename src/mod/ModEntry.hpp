#pragma once

#include "util/Version.h"

#include <optional>
#include <string>

namespace tudov
{
	struct ModEntry
	{
	  public:
		std::string namespace_;
		std::optional<Version> version;
		int32_t priority;

		explicit ModEntry(const std::string &namespace_, const std::optional<Version> &version, int32_t priority)
		    : namespace_(namespace_),
		      version(version),
		      priority(priority)
		{
		}

		bool operator==(const ModEntry &other) const
		{
			return namespace_ == other.namespace_ && version == other.version;
		}
	};
} // namespace tudov

namespace std
{
	template <>
	struct hash<tudov::ModEntry>
	{
		size_t operator()(const tudov::ModEntry &m) const
		{
			size_t h1 = hash<string>()(m.namespace_);
			size_t h2 = m.version ? hash<tudov::Version>()(*m.version) : 0;
			size_t h3 = hash<int32_t>()(m.priority);
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};
} // namespace std

#pragma once

#include "util/Version.hpp"

#include <string>

namespace tudov
{
	struct ModListedEntry
	{
	  public:
		std::string uid;
		Version version;

		explicit ModListedEntry(std::string_view uid, const Version &version)
		    : uid(std::string(uid)),
		      version(version)
		{
		}

		bool operator==(const ModListedEntry &other) const
		{
			return uid == other.uid && version == other.version;
		}
	};
} // namespace tudov

namespace std
{
	template <>
	struct hash<tudov::ModListedEntry>
	{
		std::size_t operator()(const tudov::ModListedEntry &m) const
		{
			auto h1 = hash<string>()(m.uid);
			auto h2 = hash<tudov::Version>()(m.version);
			return h1 ^ (h2 << 1);
		}
	};
} // namespace std

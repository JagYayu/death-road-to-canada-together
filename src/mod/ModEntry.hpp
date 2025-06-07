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
	};
}

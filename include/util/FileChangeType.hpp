#pragma once

#include "FileWatch.hpp"

namespace tudov
{
	enum class EFileChangeType : int
	{
		Added = static_cast<int>(filewatch::Event::added),
		Removed = static_cast<int>(filewatch::Event::removed),
		Modified = static_cast<int>(filewatch::Event::modified),
		RenamedOld = static_cast<int>(filewatch::Event::renamed_old),
		RenamedNew = static_cast<int>(filewatch::Event::renamed_new),
	};
} // namespace tudov

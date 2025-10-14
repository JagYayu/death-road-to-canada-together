/**
 * @file Util/FileChangeType.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

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

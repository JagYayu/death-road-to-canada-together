/**
 * @file i18n/TranslationPack.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Translation.hpp"

namespace tudov
{
	struct TranslationPack
	{
		std::int32_t index;
		std::int32_t priority;
		Translation translation;
	};
} // namespace tudov

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

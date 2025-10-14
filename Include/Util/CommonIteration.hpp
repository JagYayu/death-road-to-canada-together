/**
 * @file Util/CommonIteration.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <functional>

namespace tudov
{
	template <typename TReturn, typename... TArgs>
	struct CommonIteration
	{
		using Function = std::function<TReturn(TArgs...)>;

		using Iterator = void *;
	};
} // namespace tudov

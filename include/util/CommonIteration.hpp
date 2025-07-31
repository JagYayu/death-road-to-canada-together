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

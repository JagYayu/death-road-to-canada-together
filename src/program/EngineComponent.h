#pragma once

#include "Context.h"

namespace tudov
{
	class Context;

	struct IEngineComponent : IContextProvider
	{
		virtual void Initialize() noexcept
		{
		}

		virtual void Deinitialize() noexcept
		{
		}
	};
} // namespace tudov

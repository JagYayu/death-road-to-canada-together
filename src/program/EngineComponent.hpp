#pragma once

#include "Context.hpp"

namespace tudov
{
	class Context;

	struct IEngineComponent : IContextProvider
	{
		virtual ~IEngineComponent() noexcept override = default;

		virtual void Initialize() noexcept
		{
		}

		virtual void Deinitialize() noexcept
		{
		}
	};
} // namespace tudov

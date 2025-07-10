#pragma once

#include "Context.hpp"

namespace tudov
{
	class Context;
	struct ILuaAPI;

	struct IEngineComponent : public IContextProvider
	{
		~IEngineComponent() noexcept override = default;

		virtual void Initialize() noexcept;
		virtual void Deinitialize() noexcept;
		virtual void ProvideLuaAPI(ILuaAPI &luaAPI) noexcept;
	};
} // namespace tudov

#pragma once

#include "Resources.hpp"
#include "resource/Binaries.hpp"

namespace tudov
{
	template <>
	inline Resources<Binaries>::Resources() noexcept
	    : _log(Log::Get("TextureResources"))
	{
	}

	class LuaAPI;
	class ScriptEngine;

	class BinariesResources : public Resources<Binaries>
	{
		friend LuaAPI;

	  public:
		explicit BinariesResources() noexcept;
		~BinariesResources() noexcept = default;
	};
} // namespace tudov

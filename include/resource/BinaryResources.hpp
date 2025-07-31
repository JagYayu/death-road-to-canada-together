#pragma once

#include "Resources.hpp"
#include "resource/BinaryResource.hpp"

namespace tudov
{
	template <>
	inline Resources<BinaryResource>::Resources() noexcept
	    : _log(Log::Get("TextureResources"))
	{
	}

	class LuaAPI;
	class ScriptEngine;

	class ImageResources : public Resources<BinaryResource>
	{
		friend LuaAPI;

	  public:
		explicit ImageResources() noexcept;
	};
} // namespace tudov

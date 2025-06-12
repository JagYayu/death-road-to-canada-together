#pragma once

#include "ResourceManager.hpp"
#include "Texture.h"
#include "util/Defs.h"

namespace tudov
{
	using TextureManager = ResourceManager<Texture>;

	template <>
	inline TextureManager::ResourceManager()
	    : _log("TextureManager")
	{
	}
} // namespace tudov

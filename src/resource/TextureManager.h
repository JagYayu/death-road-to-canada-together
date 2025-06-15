#pragma once

#include "ResourceManager.hpp"
#include "graphic/ITexture.h"
#include "util/Defs.h"

#include <string_view>

namespace tudov
{
	template <>
	inline ResourceManager<ITexture>::ResourceManager() noexcept
	    : _log(Log::Get("TextureManager"))
	{
	}

	class ScriptEngine;

	class TextureManager : public ResourceManager<ITexture>
	{
	  public:
		TextureManager() noexcept;

		void RegisterGlobalsTo(std::string_view name, ScriptEngine &scriptEngine) noexcept;
		void UnregisterGlobalsFrom(std::string_view name, ScriptEngine &scriptEngine) noexcept;
	};
} // namespace tudov

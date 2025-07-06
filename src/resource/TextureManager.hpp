#pragma once

#include "ResourceManager.hpp"
#include "graphic/Texture.h"
#include "util/Defs.h"

#include <string_view>

namespace tudov
{
	template <>
	inline ResourceManager<Texture>::ResourceManager() noexcept
	    : _log(Log::Get("TextureManager"))
	{
	}

	class ScriptEngine;

	class TextureManager : public ResourceManager<Texture>
	{
	  public:
		TextureManager() noexcept;

		void InstallToScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;
		void UninstallFromScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;
	};
} // namespace tudov

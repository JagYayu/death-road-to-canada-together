#pragma once

#include "ResourceManager.hpp"
#include "graphic/Texture2D.h"
#include "util/Defs.h"

#include <string_view>

namespace tudov
{
	template <>
	inline ResourceManager<Texture2D>::ResourceManager() noexcept
	    : _log(Log::Get("TextureManager"))
	{
	}

	class ScriptEngine;

	class Texture2DManager : public ResourceManager<Texture2D>
	{
	  public:
		Texture2DManager() noexcept;

		void InstallToScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;
		void UninstallFromScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;
	};
} // namespace tudov

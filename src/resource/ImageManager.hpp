#pragma once

#include "ResourceManager.hpp"
#include "graphic/Image.hpp"
#include "util/Defs.hpp"

#include <string_view>

namespace tudov
{
	template <>
	inline ResourceManager<Image>::ResourceManager() noexcept
	    : _log(Log::Get("TextureManager"))
	{
	}

	class ScriptEngine;

	class ImageManager : public ResourceManager<Image>
	{
	  public:
		ImageManager() noexcept;

		void InstallToScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;
		void UninstallFromScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;
	};
} // namespace tudov

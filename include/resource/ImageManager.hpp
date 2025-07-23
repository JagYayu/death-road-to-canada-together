#pragma once

#include "ResourceManager.hpp"
#include "graphic/Image.hpp"
#include "util/Definitions.hpp"

#include <string_view>

namespace tudov
{
	template <>
	inline ResourceManager<Image>::ResourceManager() noexcept
	    : _log(Log::Get("TextureManager"))
	{
	}

	class LuaAPI;
	class ScriptEngine;

	class ImageManager : public ResourceManager<Image>
	{
		friend LuaAPI;

	  public:
		explicit ImageManager() noexcept;

		void InstallToScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;
		void UninstallFromScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;

		ImageID LuaGetID(const sol::string_view &image) noexcept;
		sol::string_view LuaGetPath(ImageID id) noexcept;
	};
} // namespace tudov

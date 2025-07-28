#pragma once

#include "ResourceManager.hpp"
#include "Text.hpp"
#include "util/Definitions.hpp"

#include <string_view>

namespace tudov
{
	template <>
	inline ResourceManager<Text>::ResourceManager() noexcept
	    : _log(Log::Get("TextManager"))
	{
	}

	class LuaAPI;
	class ScriptEngine;

	class TextManager : public ResourceManager<Text>
	{
		friend LuaAPI;

	  public:
		explicit TextManager() noexcept = default;
		~TextManager() override = default;
	};
} // namespace tudov

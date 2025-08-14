#pragma once

#include "Resources.hpp"
#include "misc/Text.hpp"

#include <string_view>

namespace tudov
{
	template <>
	inline Resources<Text>::Resources() noexcept
	    : _log(Log::Get("TextResources"))
	{
	}

	class LuaAPI;
	class ScriptEngine;

	class TextResources : public Resources<Text>
	{
		friend LuaAPI;

	  public:
		explicit TextResources() noexcept = default;
		~TextResources() override = default;
	};
} // namespace tudov

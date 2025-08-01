#pragma once

#include "Resources.hpp"
#include "Text.hpp"
#include "util/Definitions.hpp"

#include <string_view>

namespace tudov
{
	template <>
	inline Resources<TextResource>::Resources() noexcept
	    : _log(Log::Get("TextResources"))
	{
	}

	class LuaAPI;
	class ScriptEngine;

	class TextResources : public Resources<TextResource>
	{
		friend LuaAPI;

	  public:
		explicit TextResources() noexcept = default;
		~TextResources() override = default;
	};
} // namespace tudov

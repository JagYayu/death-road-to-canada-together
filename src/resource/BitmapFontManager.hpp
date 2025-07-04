#pragma once

#include "ResourceManager.hpp"
#include "graphic/BitmapFont.h"
#include "graphic/IFont.h"
#include "util/Defs.h"

#include <functional>
#include <memory>
#include <optional>
#include <string_view>
#include <unordered_map>

namespace tudov
{
	using BitmapFontManagerBase = ResourceManager<BitmapFont>;

	template <>
	inline BitmapFontManagerBase::ResourceManager() noexcept
	    : _log(Log::Get("BitmapFontManager"))
	{
	}

	class ScriptEngine;

	class BitmapFontManager : public BitmapFontManagerBase
	{
	  public:
		BitmapFontManager() noexcept;
	};
} // namespace tudov

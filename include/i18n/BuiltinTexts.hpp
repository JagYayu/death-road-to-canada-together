#pragma once

#include "util/Micros.hpp"

#include <string_view>

namespace tudov
{
	struct BuiltinTexts
	{
		TE_STATIC_CLASS(BuiltinTexts);

		static constexpr std::string_view Debug_FileSystem_Title = "Debug.FileSystem.Title";

		static constexpr std::string_view GetFallbackText(std::string_view key) noexcept
		{
			if (key == Debug_FileSystem_Title)
				return "File System";

			return "";
		}
	};
} // namespace tudov

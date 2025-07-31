#pragma once

#include <json.hpp>

namespace tudov
{
	enum class EResourceType : char
	{
		/**
		 * depreciated
		 */
		Script = 128i8,
		Binary = 0,
		Text,
		Image,
		Audio,
		Font,
	};

	inline constexpr std::string_view ResourceTypeToStringView(EResourceType resourceType) noexcept
	{
		switch (resourceType)
		{
		case EResourceType::Script:
			return "Script";
		case EResourceType::Binary:
			return "Binary";
		case EResourceType::Text:
			return "Text";
		case EResourceType::Image:
			return "Image";
		case EResourceType::Audio:
			return "Audio";
		case EResourceType::Font:
			return "Font";
		default:
			return "";
		}
	}

	NLOHMANN_JSON_SERIALIZE_ENUM(EResourceType, {
	                                                {EResourceType::Script, "Script"},
	                                                {EResourceType::Binary, "Binary"},
	                                                {EResourceType::Text, "Text"},
	                                                {EResourceType::Image, "Image"},
	                                                {EResourceType::Audio, "Audio"},
	                                                {EResourceType::Font, "Font"},
	                                            });
} // namespace tudov

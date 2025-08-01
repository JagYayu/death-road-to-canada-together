#pragma once

#include <json.hpp>

namespace tudov
{
	enum class EResourceType : char
	{
		Unknown = 0,
		Binary,
		Text,
		Image,
		Audio,
		Font,
	};

	inline constexpr std::string_view ResourceTypeToStringView(EResourceType resourceType) noexcept
	{
		switch (resourceType)
		{
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
			return "Unknown";
		}
	}

	NLOHMANN_JSON_SERIALIZE_ENUM(EResourceType, {
	                                                {EResourceType::Binary, "Binary"},
	                                                {EResourceType::Text, "Text"},
	                                                {EResourceType::Image, "Image"},
	                                                {EResourceType::Audio, "Audio"},
	                                                {EResourceType::Font, "Font"},
	                                            });
} // namespace tudov

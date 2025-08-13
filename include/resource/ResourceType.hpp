#pragma once

#include "util/Micros.hpp"

#include <json.hpp>

namespace tudov
{
	enum class EResourceType : std::uint8_t
	{
		Unknown = 0,
		Binary,
		Text,
		Image,
		Audio,
		Font,
	};

	struct ResourceType
	{
		TE_STATIC_CLASS(ResourceType);

		inline static constexpr std::string_view ToStringView(EResourceType resourceType) noexcept
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

		inline static std::string ToString(EResourceType resourceType) noexcept
		{
			return std::string(ToStringView(resourceType));
		}
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(EResourceType, {
	                                                {EResourceType::Binary, "Binary"},
	                                                {EResourceType::Text, "Text"},
	                                                {EResourceType::Image, "Image"},
	                                                {EResourceType::Audio, "Audio"},
	                                                {EResourceType::Font, "Font"},
	                                            });
} // namespace tudov

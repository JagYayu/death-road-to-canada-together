#pragma once

#include "util/Micros.hpp"

#include <cstdint>
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

	enum class EResourceTypeUsage : std::uint16_t
	{
		All = static_cast<std::underlying_type_t<EResourceType>>(-1),
		None = 0,
		Binary = 1 << (static_cast<std::underlying_type_t<EResourceType>>(EResourceType::Binary) - 1),
		Text = 1 << (static_cast<std::underlying_type_t<EResourceType>>(EResourceType::Text) - 1),
		Image = 1 << (static_cast<std::underlying_type_t<EResourceType>>(EResourceType::Image) - 1),
		Audio = 1 << (static_cast<std::underlying_type_t<EResourceType>>(EResourceType::Audio) - 1),
		Font = 1 << (static_cast<std::underlying_type_t<EResourceType>>(EResourceType::Font) - 1),
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
			case EResourceType::Unknown:
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
	                                                {EResourceType::Unknown, "Unknown"},
	                                                {EResourceType::Binary, "Binary"},
	                                                {EResourceType::Text, "Text"},
	                                                {EResourceType::Image, "Image"},
	                                                {EResourceType::Audio, "Audio"},
	                                                {EResourceType::Font, "Font"},
	                                            });
} // namespace tudov

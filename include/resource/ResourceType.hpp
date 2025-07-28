#pragma once

#include <json.hpp>

namespace tudov
{
	enum class EResourceType
	{
		Script,
		Image,
		Audio,
	};

	inline std::string_view ResourceTypeToStringView(EResourceType resourceType)
	{
		switch (resourceType)
		{
		case EResourceType::Script:
			return "Script";
		case EResourceType::Image:
			return "Image";
		case EResourceType::Audio:
			return "Audio";
		default:
			return "";
		}
	}

	NLOHMANN_JSON_SERIALIZE_ENUM(EResourceType, {
	                                                {EResourceType::Image, "Texture"},
	                                                {EResourceType::Audio, "Audio"},
	                                            });
} // namespace tudov

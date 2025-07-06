#pragma once

#include "util/Defs.h"

#include <json.hpp>

namespace tudov
{
	enum class ResourceType
	{
		Image,
		Audio,
	};

	inline std::string_view ResourceTypeToStringView(ResourceType resourceType)
	{
		switch (resourceType)
		{
		case ResourceType::Image:
			return "Image";
		case ResourceType::Audio:
			return "Audio";
		default:
			return "";
		}
	}

	NLOHMANN_JSON_SERIALIZE_ENUM(ResourceType, {
	                                               {ResourceType::Image, "Texture"},
	                                               {ResourceType::Audio, "Audio"},
	                                           });
} // namespace tudov

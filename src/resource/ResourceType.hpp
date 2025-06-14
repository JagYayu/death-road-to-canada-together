#pragma once

#include "util/Defs.h"

#include <json.hpp>

namespace tudov
{
	enum class ResourceType
	{
		Texture,
		Audio,
	};

	inline StringView ResourceTypeToStringView(ResourceType resourceType)
	{
		switch (resourceType)
		{
		case ResourceType::Texture:
			return "Texture";
		case ResourceType::Audio:
			return "Audio";
		default:
			return "";
		}
	}

	NLOHMANN_JSON_SERIALIZE_ENUM(ResourceType, {
	                                               {ResourceType::Texture, "Texture"},
	                                               {ResourceType::Audio, "Audio"},
	                                           });
} // namespace tudov

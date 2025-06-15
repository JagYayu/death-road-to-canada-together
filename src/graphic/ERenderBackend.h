#pragma once

#include "json.hpp"

namespace tudov
{
	enum class ERenderBackend
	{
		None = 0,
		SDL,
		OpenGL,
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(ERenderBackend, {
	                                                 {ERenderBackend::SDL, "SDL"},
	                                                 {ERenderBackend::OpenGL, "OpenGL"},
	                                             });
} // namespace tudov
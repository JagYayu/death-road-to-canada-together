#pragma once

#include "program/EngineComponent.hpp"

#include <string_view>

namespace tudov
{
	struct ILocalization : public IEngineComponent
	{
	};

	class Localization : public ILocalization
	{
		std::string_view GetText(std::string_view key);
	};
} // namespace tudov

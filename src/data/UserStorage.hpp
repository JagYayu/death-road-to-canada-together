#pragma once

#include "Storage.hpp"
#include "program/EngineComponent.hpp"

namespace tudov
{
	struct IUserStorage : public IStorage, public IEngineComponent
	{
		virtual ~IUserStorage() noexcept override = default;
	};
} // namespace tudov

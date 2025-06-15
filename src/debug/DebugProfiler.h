#pragma once

#include "IDebugElement.h"

namespace tudov
{
	class Engine;

	class DebugProfiler : public IDebugElement
	{
		UInt64 _prevPrefCounter;

	  public:
		Engine &engine;

		DebugProfiler(Engine &engine) noexcept;

		std::string_view GetName() noexcept override;
		void UpdateAndRender() noexcept override;
	};
} // namespace tudov
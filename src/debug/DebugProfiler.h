#pragma once

#include "CircularBuffer.hpp"
#include "IDebugElement.h"

#include <cmath>

namespace tudov
{
	class Engine;

	class DebugProfiler : public IDebugElement
	{
	  private:
		static constexpr std::size_t FramerateBufferSize = 256;

	  private:
		std::uint64_t _prevPrefCounter;
		CircularBuffer<std::float_t, FramerateBufferSize> _framerateBuffer;

	  public:
		Engine &engine;

		DebugProfiler(Engine &engine) noexcept;

		std::string_view GetName() noexcept override;
		void UpdateAndRender() noexcept override;
	};
} // namespace tudov
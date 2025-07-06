#pragma once

#include "CircularBuffer.hpp"
#include "IDebugElement.h"

#include <cmath>
#include <memory>

namespace tudov
{
	class Window;

	class DebugProfiler : public IDebugElement
	{
	  private:
		static constexpr std::size_t FramerateBufferSize = 256;

	  private:
		std::uint64_t _prevPrefCounter;
		CircularBuffer<std::float_t, FramerateBufferSize> _framerateBuffer;

	  public:
		std::weak_ptr<Window> window;

		DebugProfiler(const std::weak_ptr<Window> &window) noexcept;

		std::string_view GetName() noexcept override;
		void UpdateAndRender() noexcept override;
	};
} // namespace tudov
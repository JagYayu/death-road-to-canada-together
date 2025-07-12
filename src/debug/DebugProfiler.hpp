#pragma once

#include "CircularBuffer.hpp"
#include "IDebugElement.hpp"

#include <cmath>
#include <memory>

namespace tudov
{
	class IWindow;

	class DebugProfiler : public IDebugElement
	{
	  private:
		static constexpr std::size_t FramerateBufferSize = 256;

	  private:
		std::uint64_t _prevPrefCounter;
		CircularBuffer<std::float_t, FramerateBufferSize> _framerateBuffer;

	  public:
		static std::string_view Name() noexcept;

	  public:
		std::weak_ptr<IWindow> window;

		explicit DebugProfiler() noexcept = default;
		~DebugProfiler() noexcept = default;

		std::string_view GetName() noexcept override;
		void UpdateAndRender(IWindow &window) noexcept override;
	};
} // namespace tudov
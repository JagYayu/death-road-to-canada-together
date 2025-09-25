/**
 * @file debug/DebugProfiler.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "CircularBuffer.hpp"
#include "DebugElement.hpp"
#include "EventProfiler.hpp"

#include <cmath>
#include <memory>

namespace tudov
{
	class IWindow;
	class RuntimeEvent;

	class DebugProfiler : public IDebugElement
	{
	  protected:
		struct DebugProfilerEntry
		{
			RuntimeEvent *event;
			bool isEnabled;
			std::uint64_t index;
			std::string header;
			std::float_t (*durations)[EventProfiler::EntrySize];
			std::float_t (*memories)[EventProfiler::EntrySize];
			std::float_t avgDuration;
			std::float_t maxDuration;
			std::float_t avgMemory;
			std::float_t maxMemory;
		};

	  protected:
		static constexpr std::size_t FramerateBufferSize = 256;
		static constexpr std::size_t LuaMemoryBufferSize = 256;

	  protected:
		std::uint64_t _prevPrefCounter;
		CircularBuffer<std::float_t, FramerateBufferSize> _framerateBuffer;
		CircularBuffer<std::float_t, LuaMemoryBufferSize> _luaMemoryBuffer;

	  public:
		inline static constexpr std::string_view Name() noexcept
		{
			return "Profiler";
		}

	  public:
		std::weak_ptr<IWindow> window;

		explicit DebugProfiler() noexcept = default;
		explicit DebugProfiler(const DebugProfiler &) noexcept = delete;
		explicit DebugProfiler(DebugProfiler &&) noexcept = delete;
		DebugProfiler &operator=(const DebugProfiler &) noexcept = delete;
		DebugProfiler &operator=(DebugProfiler &&) noexcept = delete;
		~DebugProfiler() noexcept = default;

		std::string_view GetName() noexcept override;
		void UpdateAndRender(IWindow &window) noexcept override;

	  protected:
		std::vector<DebugProfilerEntry> CollectDebugProfilerEntries(IWindow &window) const noexcept;
	};
} // namespace tudov

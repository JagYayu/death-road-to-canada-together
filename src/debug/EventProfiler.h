#pragma once

#include "util/Defs.h"

#include <CircularBuffer.hpp>

#include <chrono>
#include <cstddef>
#include <tuple>

namespace tudov
{
	class ScriptEngine;

	class EventProfiler
	{
	  public:
		using TClock = std::chrono::time_point<std::chrono::high_resolution_clock>;
		using TDuration = std::chrono::nanoseconds;
		using THandlers = std::unordered_map<std::string_view, std::tuple<TDuration, size_t>>;

		struct PerfEntry
		{
			TDuration duration;
			size_t memory;
			THandlers handlers;
		};

		static constexpr UInt64 EntrySize = 256;
		using PerfEntries = CircularBuffer<PerfEntry, EntrySize>;

	  private:
		PerfEntries _perfEntries;
		TClock _time;
		size_t _memory;
		THandlers _handlers;

	  public:
		explicit EventProfiler() noexcept;

		void BeginEvent(ScriptEngine &engine) noexcept;
		void EndEvent(ScriptEngine &engine) noexcept;

		void TraceHandler(ScriptEngine &engine, std::string_view handlerName) noexcept;
		void EndHandler(ScriptEngine &engine, std::string_view handlerName) noexcept;

		const PerfEntries &GetPerfEntries() const noexcept;
	};
} // namespace tudov

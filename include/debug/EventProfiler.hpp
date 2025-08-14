#pragma once

#include "CircularBuffer.hpp"

#include <chrono>
#include <cstddef>
#include <unordered_map>

namespace tudov
{
	class IScriptEngine;

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

		static constexpr std::uint64_t EntrySize = 256;
		using PerfEntries = CircularBuffer<PerfEntry, EntrySize>;

	  private:
		std::int32_t _eventScopeCounter;
		PerfEntries _perfEntries;
		TClock _time;
		size_t _memory;
		THandlers _handlers;

	  public:
		explicit EventProfiler() noexcept;

		void BeginEvent(IScriptEngine &scriptEngine);
		void EndEvent(IScriptEngine &scriptEngine);

		void TraceHandler(IScriptEngine &scriptEngine, std::string_view handlerName) noexcept;
		void EndHandler(IScriptEngine &scriptEngine, std::string_view handlerName) noexcept;

		const PerfEntries &GetPerfEntries() const noexcept;
	};
} // namespace tudov

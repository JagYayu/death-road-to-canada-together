#include "EventProfiler.h"

#include "mod/ScriptEngine.h"

#include "util/Defs.h"

using namespace tudov;

EventProfiler::EventProfiler() noexcept
    : _perfEntries()
{
}

void EventProfiler::BeginEvent(IScriptEngine &engine) noexcept
{
	_time = std::chrono::high_resolution_clock::now();
	_memory = engine.GetMemory();
	_handlers = {};
}

void EventProfiler::EndEvent(IScriptEngine &engine) noexcept
{
	std::uint64_t memory = 0;
	if (engine.GetMemory() > _memory)
	{
		memory = engine.GetMemory() - _memory;
	}

	_perfEntries.push(PerfEntry{
	    .duration = std::chrono::high_resolution_clock::now() - _time,
	    .memory = memory,
	    .handlers = std::move(_handlers),
	});
}

void EventProfiler::TraceHandler(IScriptEngine &engine, std::string_view handlerName) noexcept
{
	auto &&time = std::chrono::high_resolution_clock::now();
	auto &&memory = engine.GetMemory();

	_handlers.try_emplace(handlerName, std::tuple<TDuration, size_t>(time - _time, _memory - memory));

	_time = time;
	_memory = memory;
}

const EventProfiler::PerfEntries &EventProfiler::GetPerfEntries() const noexcept
{
	return _perfEntries;
}

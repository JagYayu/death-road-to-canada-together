/**
 * @file debug/EventProfiler.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "debug/EventProfiler.hpp"

#include "Mod/ScriptEngine.hpp"

#include <chrono>

using namespace tudov;

EventProfiler::EventProfiler() noexcept
    : _eventScopeCounter(0),
      _perfEntries()
{
}

void EventProfiler::BeginEvent(IScriptEngine &engine)
{
	if (_eventScopeCounter != 0) [[unlikely]]
	{
		throw std::runtime_error("Bad begin event call");
	}
	++_eventScopeCounter;

	_time = std::chrono::high_resolution_clock::now();
	_memory = engine.GetMemory();
	_handlers = {};
}

void EventProfiler::EndEvent(IScriptEngine &engine)
{
	if (_eventScopeCounter != 1) [[unlikely]]
	{
		throw std::runtime_error("Bad end event call");
	}
	--_eventScopeCounter;

	std::size_t deltaMemory = 0;
	std::size_t memory = engine.GetMemory();
	if (memory > _memory)
	{
		deltaMemory = memory - _memory;
	}

	_perfEntries.push(PerfEntry{
	    .duration = std::chrono::high_resolution_clock::now() - _time,
	    .memory = deltaMemory,
	    .handlers = std::move(_handlers),
	});
}

void EventProfiler::TraceHandler(IScriptEngine &engine, std::string_view handlerName) noexcept
{
	auto time = std::chrono::high_resolution_clock::now();
	std::size_t memory = engine.GetMemory();

	_handlers.try_emplace(handlerName, std::tuple<TDuration, size_t>(time - _time, _memory - memory));

	_time = time;
	_memory = memory;
}

const EventProfiler::PerfEntries &EventProfiler::GetPerfEntries() const noexcept
{
	return _perfEntries;
}

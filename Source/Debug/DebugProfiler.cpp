/**
 * @file debug/DebugProfiler.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Debug/DebugProfiler.hpp"
#include "Debug/EventProfiler.hpp"
#include "Event/EventManager.hpp"
#include "Event/RuntimeEvent.hpp"
#include "Mod/ScriptEngine.hpp"
#include "Program/Engine.hpp"
#include "Program/Window.hpp"
#include "Util/Utils.hpp"

#include "imgui.h"

#include <algorithm>
#include <array>
#include <cfloat>
#include <limits>

using namespace tudov;

std::string_view DebugProfiler::GetName() noexcept
{
	return Name();
}

void DebugProfiler::UpdateAndRender(IWindow &window) noexcept
{
	if (ImGui::Begin("Profiler"))
	{
		_framerateBuffer.push(window.GetEngine().GetFramerate());
		{
			std::array<std::float_t, FramerateBufferSize> framerates;

			for (std::uint64_t i = FramerateBufferSize; i-- > 0;)
			{
				if (i < _framerateBuffer.size())
				{
					framerates[i] = _framerateBuffer[i];
				}
				else if (i < FramerateBufferSize - 1)
				{
					framerates[i] = framerates[i + 1];
				}
				else
				{
					framerates[i] = 0.f;
				}
			}
			ImGui::PlotLines("FPS", framerates.data(), FramerateBufferSize, 0, nullptr, 1.0f);
			ImGui::SameLine();
			ImGui::Text("Avg %.1f", Average(framerates));
			ImGui::SameLine();
			ImGui::Text("Min %.1f", *std::min_element(framerates.data(), framerates.data() + _framerateBuffer.size()));
		}

		_luaMemoryBuffer.push(window.GetScriptEngine().GetMemory());
		{
			std::float_t luaMemories[LuaMemoryBufferSize];
			std::float_t minimumMemory = std::numeric_limits<std::float_t>::max();
			for (std::uint64_t i = LuaMemoryBufferSize; i-- > 0;)
			{
				if (i < _luaMemoryBuffer.size())
				{
					std::float_t mem = _luaMemoryBuffer[i] / 1024.0f / 1024.0f;

					luaMemories[i] = mem;
					minimumMemory = std::min(minimumMemory, mem);
				}
				else if (i < LuaMemoryBufferSize - 1)
				{
					std::float_t mem = luaMemories[i + 1] / 1024.0f / 1024.0f;

					luaMemories[i] = mem;
					minimumMemory = std::min(minimumMemory, mem);
				}
				else
				{
					luaMemories[i] = 0.f;
				}
			}
			ImGui::PlotLines("MEM", luaMemories, FramerateBufferSize, 0, nullptr, 1.0f);
			ImGui::SameLine();
			ImGui::Text("%.2f MB", luaMemories[LuaMemoryBufferSize - 1]);
			ImGui::SameLine();
			ImGui::Text("Min %.2f MB", minimumMemory);
		}

		ImGui::Separator();

		std::vector<DebugProfilerEntry> entries = CollectDebugProfilerEntries(window);

		if (ImGui::Button("Trace all events"))
		{
			for (auto &entry : entries)
			{
				entry.event->EnableProfiler(false);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Trace all handlers"))
		{
			for (auto &entry : entries)
			{
				entry.event->EnableProfiler(true);
			}
		}

		for (auto &&entry : entries)
		{
			ImGui::PushID(entry.index);

			auto &&event = entry.event;

			if (ImGui::CollapsingHeader(entry.header.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				auto *profile = entry.event->GetProfile();
				if (profile != nullptr)
				{
					auto &&perfEntries = profile->eventProfiler->GetPerfEntries();
					std::float_t durations[EventProfiler::EntrySize];
					std::float_t memories[EventProfiler::EntrySize];

					for (std::uint64_t i = 0; i < EventProfiler::EntrySize; ++i)
					{
						if (i < perfEntries.size())
						{
							durations[i] = perfEntries[i].duration.count() / 1e6f;
							memories[i] = perfEntries[i].memory / 1024.0f;
						}
						else if (i != 0)
						{
							durations[i] = durations[i - 1];
							memories[i] = memories[i - 1];
						}
						else
						{
							durations[i] = 0;
							memories[i] = 0;
						}
					}

					ImGui::PlotLines("CPU", durations, EventProfiler::EntrySize);
					ImGui::SameLine();
					ImGui::Text("Avg: %.2f MS", entry.avgDuration);
					ImGui::SameLine();
					ImGui::Text("Max: %.2f MS", entry.maxDuration);

					ImGui::PlotLines("MEM", memories, EventProfiler::EntrySize);
					ImGui::SameLine();
					ImGui::Text("Avg: %.2f KB", entry.avgMemory);
					ImGui::SameLine();
					ImGui::Text("Max: %.2f KB", entry.maxMemory);

					if (ImGui::Button("Stop tracing"))
					{
						event->DisableProfiler();
					}
				}
				else
				{
					ImGui::Text("This event profiler is not available.");

					if (ImGui::Button("Trace event"))
					{
						event->EnableProfiler(false);
					}
					ImGui::SameLine();
					if (ImGui::Button("Trace handlers"))
					{
						event->EnableProfiler(true);
					}
				}
			}

			ImGui::PopID();
		}
	}
	ImGui::End();
}

std::vector<DebugProfiler::DebugProfilerEntry> DebugProfiler::CollectDebugProfilerEntries(IWindow &window) const noexcept
{
	IEventManager &eventManager = window.GetEventManager();

	std::vector<DebugProfilerEntry> entries{};
	entries.reserve(eventManager.GetRuntimeEventsCount());

	for (auto it = eventManager.BeginRuntimeEvents(); it != eventManager.EndRuntimeEvents(); ++it)
	{
		RuntimeEvent &event = *it->second;
		std::size_t index = entries.size();

		std::float_t durations[EventProfiler::EntrySize];
		std::float_t memories[EventProfiler::EntrySize];

		auto profile = event.GetProfile();
		if (profile != nullptr)
		{
			const EventProfiler::PerfEntries &perfEntries = profile->eventProfiler->GetPerfEntries();

			for (std::uint64_t i = 0; i < EventProfiler::EntrySize; ++i)
			{
				if (i < perfEntries.size())
				{
					durations[i] = perfEntries[i].duration.count() / 1e6f;
					memories[i] = perfEntries[i].memory / 1e3f;
				}
				else if (i != 0)
				{
					durations[i] = durations[i - 1];
					memories[i] = memories[i - 1];
				}
				else
				{
					durations[i] = 0;
					memories[i] = 0;
				}
			}

			entries.emplace_back(DebugProfilerEntry{
			    .event = &event,
			    .index = index,
			    .header = eventManager.GetEventNameByID(event.GetID())->data(),
			    .durations = &durations,
			    .memories = &memories,
			    .avgDuration = Average(durations),
			    .maxDuration = *std::max_element(durations, durations + perfEntries.size()),
			    .avgMemory = Average(memories),
			    .maxMemory = *std::max_element(memories, memories + perfEntries.size()),
			});
		}
		else
		{
			entries.emplace_back(DebugProfilerEntry{
			    .event = &event,
			    .index = index,
			    .header = eventManager.GetEventNameByID(event.GetID())->data(),
			    .durations = &durations,
			    .memories = &memories,
			    .avgDuration = FLT_MAX,
			    .maxDuration = FLT_MAX,
			    .avgMemory = FLT_MAX,
			    .maxMemory = FLT_MAX,
			});
		}
	}

	std::sort(entries.begin(), entries.end(), [](const DebugProfilerEntry &l, const DebugProfilerEntry &r) -> bool
	{
		{
			bool lp = l.event->GetProfile() != nullptr;
			bool rp = r.event->GetProfile() != nullptr;
			if (lp != rp)
			{
				return lp > rp;
			}
		}
		if (l.avgDuration != r.avgDuration)
		{
			return l.avgDuration > r.avgDuration;
		}
		if (l.avgMemory != r.avgMemory)
		{
			return l.avgMemory > r.avgMemory;
		}
		return l.header < r.header;
	});

	return entries;
}

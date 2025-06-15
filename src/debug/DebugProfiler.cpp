#include "DebugProfiler.h"

#include "EventProfiler.h"
#include "event/EventManager.h"
#include "event/RuntimeEvent.h"
#include "program/Engine.h"
#include "util/Defs.h"
#include "util/Utils.hpp"

#include <algorithm>
#include <cfloat>
#include <imgui.h>

using namespace tudov;

DebugProfiler::DebugProfiler(Engine &engine) noexcept
    : engine(engine)
{
}

std::string_view DebugProfiler::GetName() noexcept
{
	return "Profiler";
}

struct DebugProfilerEntry
{
	RuntimeEvent *event;
	UInt64 index;
	std::string header;
	float (*durations)[EventProfiler::EntrySize];
	float (*memories)[EventProfiler::EntrySize];
	float avgDuration;
	float maxDuration;
	float avgMemory;
	float maxMemory;
};

void DebugProfiler::UpdateAndRender() noexcept
{
	if (ImGui::Begin("Event Profilers"))
	{
		ImGui::Text("FPS: %.1f", engine.window.GetFramerate());

		std::vector<DebugProfilerEntry> entries{};

		auto &&eventManager = engine.modManager.eventManager;
		for (auto &&it = eventManager.BeginRuntimeEvents(); it != eventManager.EndRuntimeEvents(); ++it)
		{
			auto &&event = *it->second;
			auto index = entries.size();

			float durations[EventProfiler::EntrySize];
			float memories[EventProfiler::EntrySize];

			if (event.GetProfile())
			{
				auto &&perfEntries = event.GetProfile()->get().eventProfiler.GetPerfEntries();

				for (UInt64 i = 0; i < EventProfiler::EntrySize; ++i)
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

		std::sort(entries.begin(), entries.end(), [](auto &&l, auto &&r) -> bool
		{
			if (l.avgDuration != r.avgDuration)
			{
				return l.avgDuration < r.avgDuration;
			}
			if (l.avgMemory != r.avgMemory)
			{
				return l.avgMemory < r.avgMemory;
			}
			return l.header < r.header;
		});

		for (auto &&entry : entries)
		{
			ImGui::PushID(entry.index);

			auto &&event = entry.event;

			if (ImGui::CollapsingHeader(entry.header.c_str()))
			{
				auto &&profile = entry.event->GetProfile();
				if (profile)
				{
					auto &&perfEntries = profile->get().eventProfiler.GetPerfEntries();
					float durations[EventProfiler::EntrySize];
					float memories[EventProfiler::EntrySize];

					for (UInt64 i = 0; i < EventProfiler::EntrySize; ++i)
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

					ImGui::PlotLines("CPU", durations, EventProfiler::EntrySize);
					ImGui::SameLine();
					ImGui::Text("Avg: %.2fms", entry.avgDuration);
					ImGui::SameLine();
					ImGui::Text("Max: %.2fms", entry.maxDuration);

					ImGui::PlotLines("MEM", memories, EventProfiler::EntrySize);
					ImGui::SameLine();
					ImGui::Text("Avg: %.2fkb", entry.avgMemory);
					ImGui::SameLine();
					ImGui::Text("Max: %.2fkb", entry.maxMemory);

					if (ImGui::Button("Stop Trace"))
					{
						event->DisableProfiler();
					}
				}
				else
				{
					ImGui::Text("This event profiler is not available.");

					if (ImGui::Button("Trace Event"))
					{
						event->EnableProfiler(false);
					}
					ImGui::SameLine();
					if (ImGui::Button("Trace Handlers"))
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

#include "DebugProfiler.hpp"

#include "EventProfiler.hpp"
#include "event/EventManager.hpp"
#include "program/Engine.hpp"
#include "program/Window.hpp"
#include "util/Utils.hpp"

#include "imgui.h"

#include <algorithm>
#include <cfloat>

using namespace tudov;

std::string_view DebugProfiler::GetName() noexcept
{
	return "Profiler";
}

struct DebugProfilerEntry
{
	RuntimeEvent *event;
	std::uint64_t index;
	std::string header;
	float (*durations)[EventProfiler::EntrySize];
	float (*memories)[EventProfiler::EntrySize];
	float avgDuration;
	float maxDuration;
	float avgMemory;
	float maxMemory;
};

void DebugProfiler::UpdateAndRender(const std::shared_ptr<IWindow> &window) noexcept
{
	if (ImGui::Begin("Profiler"))
	{
		_framerateBuffer.push(window->GetEngine().GetFramerate());
		{
			float framerates[FramerateBufferSize];
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
			ImGui::PlotLines("FPS", framerates, FramerateBufferSize, 0, nullptr, 1.0F);
			ImGui::SameLine();
			ImGui::Text("Avg %.1f", Average(framerates));
			ImGui::SameLine();
			ImGui::Text("Min %.1f", *std::min_element(framerates, framerates + _framerateBuffer.size()));
			// ImGui::Text("FPS: %.1f", engine.window.GetFramerate());
		}

		std::vector<DebugProfilerEntry> entries{};

		auto &&eventManager = window->GetEventManager();
		for (auto &&it = eventManager->BeginRuntimeEvents(); it != eventManager->EndRuntimeEvents(); ++it)
		{
			auto &&event = *it->second;
			auto index = entries.size();

			float durations[EventProfiler::EntrySize];
			float memories[EventProfiler::EntrySize];

			if (event.GetProfile())
			{
				auto &&perfEntries = event.GetProfile()->get().eventProfiler.GetPerfEntries();

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
				    .header = eventManager->GetEventNameByID(event.GetID())->data(),
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
				    .header = eventManager->GetEventNameByID(event.GetID())->data(),
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

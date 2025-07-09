#include "DebugManager.hpp"

#include "DebugConsole.hpp"
#include "DebugProfiler.hpp"
#include "program/Window.hpp"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <memory>

using namespace tudov;

DebugManager::DebugManager() noexcept
    : console(std::make_shared<DebugConsole>()),
      profiler(std::make_shared<DebugProfiler>()),
      _elements(),
      _shownElements()
{
	_elements = {
	    console,
	    profiler,
	};
}

void DebugManager::AddElement(const std::shared_ptr<IDebugElement> &element) noexcept
{
	auto &&name = element->GetName();
	for (auto &&element : _elements)
	{
		if (name == element->GetName())
		{
			return;
		}
	}
	_elements.emplace_back(element);
}

void DebugManager::RemoveElement(std::string_view element) noexcept
{
	for (auto &&it = _elements.begin(); it != _elements.end(); ++it)
	{
		if (it->get()->GetName() == element)
		{
			_elements.erase(it);
			break;
		}
	}
	_shownElements.erase(element);
}

void DebugManager::UpdateAndRender(const std::shared_ptr<IWindow> &window) noexcept
{
	ImGui_ImplSDL3_NewFrame();
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar())
	{
		ImGui::BeginChild("##DebugManager", ImVec2(0, 30), false, ImGuiWindowFlags_HorizontalScrollbar);

		for (size_t i = 0; i < _elements.size(); ++i)
		{
			ImGui::PushID(i);

			auto &&name = _elements[i]->GetName();
			if (ImGui::Button(name.data()))
			{
				if (_shownElements.contains(name))
				{
					_shownElements.erase(name);
				}
				else
				{
					_shownElements.emplace(name);
				}
			}
			ImGui::SameLine();

			ImGui::PopID();
		}

		ImGui::EndChild();
		ImGui::EndMainMenuBar();
	}

	for (auto &&element : _elements)
	{
		if (_shownElements.contains(element->GetName()))
		{
			element->UpdateAndRender(window);
		}
	}

	ImGui::EndFrame();

	ImGui::Render();
}

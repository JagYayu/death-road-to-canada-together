#include "DebugManager.hpp"

#include "DebugConsole.hpp"
#include "DebugProfiler.hpp"
#include "program/Window.hpp"

#include "imgui.h"
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

IDebugElement *DebugManager::GetElement(std::string_view elementName) noexcept
{
	for (auto &&element : _elements)
	{
		if (elementName == element->GetName())
		{
			return element.get();
		}
	}
	return nullptr;
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

bool DebugManager::RemoveElement(std::string_view elementName) noexcept
{
	for (auto &&it = _elements.begin(); it != _elements.end(); ++it)
	{
		if (it->get()->GetName() == elementName)
		{
			_elements.erase(it);
			_shownElements.erase(elementName);
			return true;
		}
	}
	return false;
}

void DebugManager::UpdateAndRender(IWindow &window) noexcept
{
	if (ImGui::BeginMainMenuBar())
	{
		// ImGui::SetWindowFontScale(window.GetDisplayScale());

		if (ImGui::BeginChild("##DebugManager", ImVec2(0, 48), false, ImGuiWindowFlags_HorizontalScrollbar))
		{
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
		}

		ImGui::EndMainMenuBar();
	}

	for (auto &&element : _elements)
	{
		if (_shownElements.contains(element->GetName()))
		{
			element->UpdateAndRender(window);
		}
	}
}

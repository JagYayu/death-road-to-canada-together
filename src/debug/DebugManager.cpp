/**
 * @file debug/DebugManager.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "debug/DebugManager.hpp"

#include "debug/DebugConsole.hpp"
#include "debug/DebugFileSystem.hpp"
#include "debug/DebugLog.hpp"
#include "debug/DebugProfiler.hpp"
#include "debug/DebugScripts.hpp"
#include "program/Window.hpp"

#include "imgui.h"

#include <memory>

using namespace tudov;

DebugManager::DebugManager() noexcept
    : console(std::make_shared<DebugConsole>()),
      fileSystem(std::make_shared<DebugFileSystem>()),
      log(std::make_shared<DebugLog>()),
      profiler(std::make_shared<DebugProfiler>()),
      scripts(std::make_shared<DebugScripts>()),
      _elements(),
      _shownElements()
{
	_elements = {
	    console,
	    fileSystem,
	    log,
	    profiler,
	    scripts,
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
		std::float_t scale = window.GetDisplayScale();

		if (ImGui::BeginChild("##DebugManager", ImVec2(0, 48 * scale), false, ImGuiWindowFlags_HorizontalScrollbar))
		{
			for (size_t i = 0; i < _elements.size(); ++i)
			{
				ImGui::PushID(i);

				std::shared_ptr<IDebugElement> &element = _elements[i];
				std::string_view name = element->GetName();

				if (ImGui::Button(name.data()))
				{
					if (_shownElements.contains(name))
					{
						_shownElements.erase(name);
						element->OnClosed(window);
					}
					else
					{
						_shownElements.emplace(name);
						element->OnOpened(window);
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

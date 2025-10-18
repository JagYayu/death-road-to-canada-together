/**
 * @file debug/DebugFileSystem.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Debug/DebugFileSystem.hpp"

#include "Data/PathType.hpp"
#include "Data/VirtualFileSystem.hpp"
#include "I18n/BuiltinTexts.hpp"
#include "I18n/Localization.hpp"
#include "Util/EnumFlag.hpp"

#include "imgui.h"

#include <algorithm>
#include <filesystem>
#include <vector>

using namespace tudov;

DebugFileSystem::DebugFileSystem() noexcept
    : _browsePath()
{
}

DebugFileSystem::~DebugFileSystem() noexcept
{
}

std::string_view DebugFileSystem::GetName() noexcept
{
	return Name();
}

void DebugFileSystem::OnOpened(IWindow &window) noexcept
{
	_currentPath = "";
	_selectedFile = "";
	_refresh = true;
}

void DebugFileSystem::OnClosed(IWindow &window) noexcept
{
	_currentPath = "";
	_selectedFile = "";
	_entries = {};
}

void DebugFileSystem::UpdateAndRender(IWindow &window) noexcept
{
	ILocalization &localization = window.GetLocalization();
	VirtualFileSystem &virtualFileSystem = window.GetVirtualFileSystem();

	// apply refresh, update current directory's entries.
	if (_refresh) [[unlikely]]
	{
		_refresh = false;
		Refresh(virtualFileSystem);
	}

	std::float_t scale = window.GetGUIScale();

	ImGui::Begin(localization.GetText(BuiltinTexts::Debug_FileSystem_Title).data());

	ImGui::PushItemWidth(-80 * scale);
	if (ImGui::InputText("##Path", _browsePath.data(), _browsePath.size(), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		_currentPath = _browsePath.data();
		_refresh = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Go"))
	{
		_currentPath = _browsePath.data();
		_refresh = true;
	}
	ImGui::SameLine();
	if (_currentPath != "/")
	{
		if (ImGui::Button("Up"))
		{
			_currentPath = _currentPath.parent_path();
			_refresh = true;
		}
	}
	ImGui::PopItemWidth();

	ImGui::Separator();

	if (ImGui::BeginTable("FileTable", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY))
	{
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
		ImGui::TableSetupColumn("Date modified", ImGuiTableColumnFlags_WidthFixed, 120 * scale);
		ImGui::TableSetupColumn("Size (KB)", ImGuiTableColumnFlags_WidthFixed, 70 * scale);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 110 * scale);
		ImGui::TableHeadersRow();

		std::int32_t id = 0;
		for (auto &entry : _entries)
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			bool isSelected = false;

			ImGui::PushID(id++);
			if (ImGui::Selectable("##row", isSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
			{
				if (entry.isDirectory)
				{
					_currentPath = (std::filesystem::path(_currentPath) / entry.name).string();

					_refresh = true;
				}
			}
			ImGui::PopID();

			if (ImGui::IsItemHovered())
			{
				ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(ImGuiCol_HeaderHovered));
			}

			ImGui::SameLine();
			if (entry.isDirectory)
			{
				ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.6f, 1.0f), "%s", entry.name.data());
			}
			else
			{
				ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "%s", entry.name.data());
			}

			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted(entry.dateModified.data());

			ImGui::TableSetColumnIndex(2);
			ImGui::Text("%.2f", entry.sizeKB);

			ImGui::TableSetColumnIndex(3);
			ImGui::TextUnformatted(entry.type.data());
		}

		ImGui::EndTable();
	}

	ImGui::End();
}

void DebugFileSystem::Refresh(VirtualFileSystem &vfs) noexcept
{
	_entries.clear();

	std::vector<VirtualFileSystem::ListEntry> list;

	std::string browsePath = _currentPath.generic_string();
	std::strncpy(_browsePath.data(), browsePath.c_str(), _browsePath.size() - 1);
	_browsePath[std::min(browsePath.size(), _browsePath.size() - 1)] = '\0';

	if (vfs.GetPathType(_currentPath) != EPathType::Directory)
	{
		return;
	}
	else
	{
		auto options = EnumFlag::BitOr(
		    EPathListOption::File,
		    EPathListOption::Directory,
		    EPathListOption::Sorted);

		list = vfs.List(_currentPath, options);
	}

	for (auto &[path, isDirectory] : list)
	{
		std::filesystem::path fullPath = _currentPath / path;
		std::string dateModified;
		std::string type;
		std::size_t bytes = vfs.GetPathSize(fullPath);

		{
			auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(vfs.GetPathDateModified(fullPath));
			dateModified = std::format("{:%Y/%m/%d %H:%M:%S}", seconds);
		}

		if (isDirectory)
		{
			type = "Folder";
		}
		else
		{
			type = ResourceType::ToString(vfs.GetFileResourceType(fullPath)) + " File";
		}

		_entries.emplace_back(
		    path,
		    isDirectory,
		    dateModified,
		    type,
		    static_cast<std::double_t>(bytes) / 1024.0);
	}
}

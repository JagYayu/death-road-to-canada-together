/**
 * @file data/GlobalStorageLocation_Win32.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Data/GlobalStorageLocation.hpp"

#ifdef _WIN32

#	include <shlobj.h>

#	pragma comment(lib, "shell32.lib")

using namespace tudov;

bool GlobalStorageLocation::IsAccessible() noexcept
{
	return true;
}

std::filesystem::path GlobalStorageLocation::GetPath(EGlobalStorageLocation location) noexcept
{
	switch (location)
	{
	case EGlobalStorageLocation::Application:
	{
		wchar_t buffer[MAX_PATH];
		GetModuleFileNameW(nullptr, buffer, MAX_PATH);
		std::filesystem::path exePath(buffer);
		return exePath.parent_path();
	}
	case EGlobalStorageLocation::User:
	{
		wchar_t *roamingPath = nullptr;
		if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &roamingPath) != S_OK)
		{
			CoTaskMemFree(roamingPath);
			return "";
		}

		wchar_t buffer[MAX_PATH];
		GetModuleFileNameW(nullptr, buffer, MAX_PATH);
		std::filesystem::path exePath(buffer);
		std::wstring appName = exePath.stem().wstring();

		std::filesystem::path userPath(roamingPath);
		userPath /= appName;

		CoTaskMemFree(roamingPath);

		std::filesystem::create_directories(userPath);

		return userPath;
	}
	default:
		return "";
	}
}

#endif
/**
 * @file program/Tudov.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "program/Tudov.hpp"

#include "data/Config.hpp"
#include "program/Application.hpp"
#include "program/Engine.hpp"
#include "program/MainArgs.hpp"
#include "System/Log.hpp"

#include "SDL3/SDL_messagebox.h"

#include <Windows.h>
#include <libloaderapi.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <winbase.h>

using namespace tudov;

static std::shared_ptr<Engine> _engine = nullptr;
static bool _engineInitialized = false;

bool FatalError_MessageBox(std::string_view errorMessage) noexcept
{
	constexpr int buttonNo = 0;
	constexpr int buttonYes = 1;

	SDL_MessageBoxButtonData buttons[] = {
	    {0, buttonNo, "No"},
	    {0, buttonYes, "Yes"},
	};

	std::string message = std::format("Fatal error has occurred:\n{}\nDo you want to terminate the program?", errorMessage);

	SDL_MessageBoxData messageboxdata = {
	    SDL_MESSAGEBOX_ERROR,
	    nullptr,
	    "Tudov Engine",
	    message.data(),
	    SDL_arraysize(buttons),
	    buttons,
	    nullptr,
	};

	int id;
	SDL_ShowMessageBox(&messageboxdata, &id);
	return id == buttonYes;
}

void Tudov::FatalError(std::string_view errorMessage) noexcept
{
	if (FatalError_MessageBox(errorMessage))
	{
		Log::Exit();
		abort();
	}
}

std::shared_ptr<Application> Tudov::GetApplication() noexcept
{
	if (_engine == nullptr) [[unlikely]]
	{
		if (_engineInitialized)
		{
			return nullptr;
		}

		_engine = std::make_shared<Engine>();
		_engineInitialized = true;
	}

	return _engine;
}

void Tudov::ReleaseApplication() noexcept
{
	if (_engine != nullptr)
	{
		_engine = nullptr;
	}
}

static std::unique_ptr<Config> _config;

Config &Tudov::GetConfig() noexcept
{
	if (_config == nullptr) [[unlikely]]
	{
		FatalError("Attempt to get config before initialization");
	}

	return *_config;
}

void Tudov::InitConfig() noexcept
{
	_config = std::make_unique<Config>();
}

static MainArgs _mainArgs = MainArgs();
static bool _mainArgsInitialized = false;

const MainArgs &Tudov::GetMainArgs() noexcept
{
	return _mainArgs;
}

void Tudov::InitMainArgs(int argc, char **argv)
{
	if (_mainArgsInitialized) [[unlikely]]
	{
		throw std::runtime_error("main args have already been initialized!");
	}

	_mainArgs = MainArgs(argc, argv);
	_mainArgsInitialized = true;
}

// static std::wstring dllDirectory;

// bool Tudov::Windows_AddDllDirectory() noexcept
// {
// 	WCHAR exePath[MAX_PATH];
// 	GetModuleFileNameW(nullptr, exePath, MAX_PATH);

// 	WCHAR *lastBackslash = wcsrchr(exePath, L'\\');
// 	if (lastBackslash)
// 	{
// 		*lastBackslash = L'\0';

// 		WCHAR crtPath[MAX_PATH];
// 		swprintf_s(crtPath, MAX_PATH, L"%s\\crt", exePath);

// 		if (AddDllDirectory(crtPath) == 0)
// 		{
// 			DWORD error = GetLastError();
// 			wprintf(L"Failed to add DLL directory: %s, error: %d\n", crtPath, error);
// 		}
// 		else
// 		{
// 			wprintf(L"Successfully added DLL directory: %s\n", crtPath);
// 		}
// 	}
// 	return true;
// }

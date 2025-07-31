#include "program/Tudov.hpp"

#include "program/Application.hpp"
#include "program/Engine.hpp"
#include "program/MainArgs.hpp"
#include "util/Log.hpp"

#include "SDL3/SDL_messagebox.h"

#include <Windows.h>
#include <codecvt>
#include <cstdlib>
#include <libloaderapi.h>
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
	    NULL,
	    "Tudov Engine",
	    message.data(),
	    SDL_arraysize(buttons),
	    buttons,
	    NULL,
	};

	int id;
	SDL_ShowMessageBox(&messageboxdata, &id);
	return id == 0;
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

static MainArgs _mainArgs = MainArgs();
static bool _mainArgsInitialized = false;

const MainArgs &Tudov::GetMainArgs() noexcept
{
	return _mainArgs;
}

void Tudov::InitMainArgs(const MainArgs &mainArgs)
{
	if (_mainArgsInitialized) [[unlikely]]
	{
		throw std::runtime_error("main args have already been initialized!");
	}

	_mainArgs = mainArgs;
	_mainArgsInitialized = true;
}

// static std::wstring dllDirectory;

// bool Tudov::Windows_AddDllDirectory() noexcept
// {
// 	WCHAR exePath[MAX_PATH];
// 	GetModuleFileNameW(NULL, exePath, MAX_PATH);

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

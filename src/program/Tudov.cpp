#include "program/Tudov.hpp"

#include "program/Engine.hpp"
#include "program/MainArgs.hpp"
#include "util/Log.hpp"

#include "SDL3/SDL_messagebox.h"

#include <cstdlib>
#include <stdexcept>

using namespace tudov;

static std::shared_ptr<Engine> _engine = nullptr;
static bool _engineInitialized = false;

bool FatalError_MessageBox(std::string_view errorMessage) noexcept
{
	constexpr int buttonYes = 0;
	constexpr int buttonNo = 1;

	SDL_MessageBoxButtonData buttons[] = {
	    {0, buttonYes, "Yes"},
	    {0, buttonNo, "No"},
	};

	std::string message = std::format("发生致命错误:\n{}\n是否终止程序?", errorMessage);

	SDL_MessageBoxData messageboxdata = {
	    SDL_MESSAGEBOX_ERROR,
	    NULL,
	    "错误",
	    message.data(),
	    SDL_arraysize(buttons),
	    buttons,
	    NULL,
	};

	int id;
	SDL_ShowMessageBox(&messageboxdata, &id);
	return id == 0;
}

void FatalError(std::string_view errorMessage) noexcept
{
	if (FatalError_MessageBox(errorMessage))
	{
		_engine = nullptr;
		Log::Exit();
		abort();
	}
}

std::shared_ptr<Application> GetApplication() noexcept
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

MainArgs &GetMainArgs()
{
	return _mainArgs;
}

void InitMainArgs(const MainArgs &mainArgs)
{
	if (_mainArgsInitialized) [[unlikely]]
	{
		throw std::runtime_error("main args have already been initialized!");
	}

	_mainArgs = mainArgs;
	_mainArgsInitialized = true;
}

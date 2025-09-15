/**
 * @file program/Main.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "data/Config.hpp"
#include "data/Constants.hpp"
#include "event/AppEvent.hpp"
#include "graphic/GUI.hpp"
#include "program/CrashReporter.hpp"
#include "program/Engine.hpp"
#include "program/Tudov.hpp"
#include "system/Log.hpp"
#include "system/LogMicros.hpp"
#include "test/TestGPURendering.hpp"

#include "SDL3_ttf/SDL_ttf.h"

#include <memory>

#define SDL_MAIN_USE_CALLBACKS

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_main.h"

using namespace tudov;

const char *CategoryToCStr(int category) noexcept
{
	switch (category)
	{
	case SDL_LOG_CATEGORY_APPLICATION:
		return "Application";
	case SDL_LOG_CATEGORY_ERROR:
		return "Error";
	case SDL_LOG_CATEGORY_ASSERT:
		return "Assert";
	case SDL_LOG_CATEGORY_SYSTEM:
		return "System";
	case SDL_LOG_CATEGORY_AUDIO:
		return "Audio";
	case SDL_LOG_CATEGORY_VIDEO:
		return "Video";
	case SDL_LOG_CATEGORY_RENDER:
		return "Render";
	case SDL_LOG_CATEGORY_INPUT:
		return "Input";
	case SDL_LOG_CATEGORY_TEST:
		return "Test";
	case SDL_LOG_CATEGORY_GPU:
		return "Gpu";
	default:
		return "SDL3";
	}
}

void SDLLogOutputCallback(void *userdata, int category, SDL_LogPriority priority, const char *message) noexcept
{
	const std::shared_ptr<Log> &TE_L_log = Log::Get(std::string("@SDL.") + CategoryToCStr(category));
	switch (priority)
	{
	case SDL_LOG_PRIORITY_TRACE:
	case SDL_LOG_PRIORITY_VERBOSE:
		TE_L_TRACE("{}", message);
	case SDL_LOG_PRIORITY_DEBUG:
		TE_L_DEBUG("{}", message);
		break;
	case SDL_LOG_PRIORITY_INFO:
		TE_L_INFO("{}", message);
		break;
	case SDL_LOG_PRIORITY_WARN:
		TE_L_WARN("{}", message);
		break;
	case SDL_LOG_PRIORITY_ERROR:
		TE_L_ERROR("{}", message);
	case SDL_LOG_PRIORITY_CRITICAL:
		TE_L_FATAL("{}", message);
		break;
	default:
		break;
	}
}

bool CommonInit(int argc, char **argv) noexcept
{
	CrashReporter::InitializeCrashReporter();

	// This will also initialize Logging system.
	const std::shared_ptr<Log> &TE_L_log = Log::Get("Main");

	Tudov::InitConfig();

	TE_L_INFO("{}", "Application initializing ...");

	// Logging application constants.
	TE_L_INFO("AppName = {}", Constants::AppName);
	TE_L_INFO("AppOrganization = {}", Constants::AppOrganization);
	TE_L_INFO("DataConfigFile = {}", Constants::DataConfigFile);
	TE_L_INFO("DataUserDirectoryPrefix = {}", Constants::DataUserDirectoryPrefix);
	TE_L_INFO("DataDeveloperAssetsDirectory = {}", Constants::DataDeveloperAssetsDirectory);
	TE_L_INFO("DataVirtualStorageRootApp = {}", Constants::DataVirtualStorageRootApp);
	TE_L_INFO("DataVirtualStorageRootMods = {}", Constants::DataVirtualStorageRootMods);
	TE_L_INFO("DataVirtualStorageRootUser = {}", Constants::DataVirtualStorageRootUser);
	TE_L_INFO("NetworkChannelsLimit = {}", Constants::NetworkChannelsLimit);
	TE_L_INFO("NetworkServerMaximumClients = {}", Constants::NetworkServerMaximumClients);
	TE_L_INFO("NetworkServerPassword = {}", Constants::NetworkServerPassword);
	TE_L_INFO("NetworkServerTitle = {}", Constants::NetworkServerTitle);
	TE_L_INFO("WindowTitle = {}", Constants::WindowTitle);
	TE_L_INFO("WindowWidth = {}", Constants::WindowWidth);
	TE_L_INFO("WindowHeight = {}", Constants::WindowHeight);

	Tudov::InitMainArgs(argc, argv);

	// Initialize SDL3.
	SDL_SetLogOutputFunction(SDLLogOutputCallback, nullptr);
	if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS | SDL_INIT_SENSOR | SDL_INIT_CAMERA)) [[unlikely]]
	{
		TE_L_FATAL("SDL3 failed to initialize", SDL_GetError());
	}
	TE_L_INFO("SDL3 initialized");

	// Initialize SDL3_TTF.
	TTF_Init();
	TE_L_INFO("SDL3 initialized");

	return false;
}

void CommonQuit(SDL_AppResult result) noexcept
{
	GUI::Quit();

	Tudov::GetConfig().Save();

	{ // Quit logging system.
		const Log *TE_L_log = &Log::GetInstance();
		TE_L_INFO("Application quit, result code: {}", std::int32_t(result));
		Log::Quit();
	}
}

#ifdef TE_TEST_GPU_RENDERING

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
	if (CommonInit(argc, argv))
	{
		return SDL_APP_FAILURE;
	}

	TestGPURendering::Init(appstate);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
	return static_cast<SDL_AppResult>(TestGPURendering::Iterate(appstate));
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
	return static_cast<SDL_AppResult>(TestGPURendering::Event(appstate, event));
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	TestGPURendering::Quit(appstate);
	CommonQuit(result);
}

#else

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
	if (CommonInit(argc, argv))
	{
		return SDL_APP_FAILURE;
	}

	std::shared_ptr<Application> app = Tudov::GetApplication();
	if (app != nullptr)
	{
		app->Initialize();
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
	auto &&app = Tudov::GetApplication();
	if (app == nullptr) [[unlikely]]
	{
		return SDL_APP_FAILURE;
	}

	try
	{
		return app->Tick() ? SDL_APP_CONTINUE : SDL_APP_SUCCESS;
	}
	catch (const std::exception &e)
	{
		TE_G_FATAL("Main", "Unhandled exception in application iteration: {}", e.what());
	}

	return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
	auto &&app = Tudov::GetApplication();
	if (app == nullptr)
	{
		return SDL_APP_FAILURE;
	}

	try
	{
		AppEvent appEvent{*event};
		app->Event(appEvent);

		return SDL_APP_CONTINUE;
	}
	catch (const std::exception &e)
	{
		TE_G_FATAL("Main", "Unhandled exception in application event handling: {}", e.what());
		return SDL_APP_FAILURE;
	}
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	{
		auto &&app = Tudov::GetApplication();
		if (app == nullptr)
		{
			return;
		}

		if (result != SDL_APP_FAILURE)
		{
			app->Deinitialize();
		}
		Tudov::ReleaseApplication();
	}

	CommonQuit(result);
}

#endif

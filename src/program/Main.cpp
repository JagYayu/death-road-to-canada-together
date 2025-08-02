#include "data/Constants.hpp"
#include "graphic/GUI.hpp"
#include "program/Engine.hpp"
#include "program/Tudov.hpp"
#include "test/TestGPURendering.hpp"
#include "util/Log.hpp"

#define SDL_MAIN_USE_CALLBACKS

#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_messagebox.h"

#include <memory>

using namespace tudov;

void SDLLogOutputCallback(void *userdata, int category, SDL_LogPriority priority, const char *message) noexcept
{
	const char *cate = "";
	switch (category)
	{
	case SDL_LOG_CATEGORY_APPLICATION:
		cate = "Application";
		break;
	case SDL_LOG_CATEGORY_ERROR:
		cate = "Error";
		break;
	case SDL_LOG_CATEGORY_ASSERT:
		cate = "Assert";
		break;
	case SDL_LOG_CATEGORY_SYSTEM:
		cate = "System";
		break;
	case SDL_LOG_CATEGORY_AUDIO:
		cate = "Audio";
		break;
	case SDL_LOG_CATEGORY_VIDEO:
		cate = "Video";
		break;
	case SDL_LOG_CATEGORY_RENDER:
		cate = "Render";
		break;
	case SDL_LOG_CATEGORY_INPUT:
		cate = "Input";
		break;
	case SDL_LOG_CATEGORY_TEST:
		cate = "Test";
		break;
	case SDL_LOG_CATEGORY_GPU:
		cate = "Gpu";
		break;
	}

	auto &&log = Log::Get(std::string("@SDL.") + cate);
	switch (priority)
	{
	case SDL_LOG_PRIORITY_TRACE:
	case SDL_LOG_PRIORITY_VERBOSE:
		log->Trace("{}", message);
	case SDL_LOG_PRIORITY_DEBUG:
		log->Debug("{}", message);
		break;
	case SDL_LOG_PRIORITY_INFO:
		log->Info("{}", message);
		break;
	case SDL_LOG_PRIORITY_WARN:
		log->Warn("{}", message);
		break;
	case SDL_LOG_PRIORITY_ERROR:
		log->Error("{}", message);
	case SDL_LOG_PRIORITY_CRITICAL:
		log->Fatal("{}", message);
		break;
	default:
		break;
	}
}

bool CommonInit(int argc, char **argv) noexcept
{
	auto log = Log::Get("Main");

	log->Info("Application initializing ...");

	log->Debug("AppName = {}", Constants::AppName);
	log->Debug("AppOrganization = {}", Constants::AppOrganization);
	log->Debug("DataConfigFile = {}", Constants::DataConfigFile);
	log->Debug("DataUserDirectoryPrefix = {}", Constants::DataUserDirectoryPrefix);
	log->Debug("DataDeveloperAssetsDirectory = {}", Constants::DataDeveloperAssetsDirectory);
	log->Debug("DataVirtualStorageRootApp = {}", Constants::DataVirtualStorageRootApp);
	log->Debug("DataVirtualStorageRootMods = {}", Constants::DataVirtualStorageRootMods);
	log->Debug("DataVirtualStorageRootUser = {}", Constants::DataVirtualStorageRootUser);
	log->Debug("NetworkChannelsLimit = {}", Constants::NetworkChannelsLimit);
	log->Debug("NetworkServerMaximumClients = {}", Constants::NetworkServerMaximumClients);
	log->Debug("NetworkServerPassword = {}", Constants::NetworkServerPassword);
	log->Debug("NetworkServerTitle = {}", Constants::NetworkServerTitle);
	log->Debug("WindowTitle = {}", Constants::WindowTitle);
	log->Debug("WindowWidth = {}", Constants::WindowWidth);
	log->Debug("WindowHeight = {}", Constants::WindowHeight);

	Tudov::InitMainArgs(argc, argv);

	SDL_SetLogOutputFunction(SDLLogOutputCallback, nullptr);
	if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS | SDL_INIT_SENSOR | SDL_INIT_CAMERA)) [[unlikely]]
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "SDL3 failed to initialize", nullptr);
		return true;
	}
	log->Info("SDL3 initialized");

	for (auto index = 0; index < SDL_GetNumGPUDrivers(); ++index)
	{
		log->Info("GPU Driver {}: {}", index + 1, SDL_GetGPUDriver(index));
	}

	return false;
}

void CommonQuit(SDL_AppResult result) noexcept
{
	GUI::Quit();

	Log::GetInstance().Info("Application quit, result code: {}", std::int32_t(result));
	Log::Quit();
}

// #define TE_TEST_GPU_RENDERING

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
	if (app == nullptr)
	{
		return SDL_APP_FAILURE;
	}
	return app->Tick() ? SDL_APP_CONTINUE : SDL_APP_SUCCESS;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
	auto &&app = Tudov::GetApplication();
	if (app == nullptr)
	{
		return SDL_APP_FAILURE;
	}

	app->Event(static_cast<void *>(event));
	return SDL_APP_CONTINUE;
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

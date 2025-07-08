#include "program/Engine.h"

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_messagebox.h"
#include "imgui.h"

using namespace tudov;

void SDLLogOutputCallback(void *userdata, int category, SDL_LogPriority priority, const char *message)
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

int main(int argc, char **args)
{
	if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS | SDL_INIT_SENSOR | SDL_INIT_CAMERA))
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing SDL3", nullptr);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	{
		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	}

	Engine(MainArgs{argc, args}).Run();

	ImGui::DestroyContext();

	SDL_Quit();

	Log::Exit();

	return 0;
}

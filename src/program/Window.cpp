#include "Window.hpp"

#include "SDL3/SDL_video.h"
#include "event/EventManager.hpp"
#include "graphic/Renderer.hpp"
#include "mod/ScriptEngine.hpp"
#include "program/Engine.hpp"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"

#include <cmath>
#include <memory>

using namespace tudov;

Window::Window(Context &context, std::string_view logName) noexcept
    : _context(context),
      renderer(std::make_shared<Renderer>(*this)),
      _log(Log::Get(logName)),
      _sdlWindow(nullptr),
      _shouldClose(false)
{
}

Window::~Window() noexcept
{
	if (_sdlWindow)
	{
		SDL_DestroyWindow(_sdlWindow);
	}
}

// ILuaAPI::TInstallation Window::windowLuaAPIInstallation = [](sol::state &lua)
// {
// 	lua.new_usertype<Window>("tudov_Window",
// 	                         "renderer", &Window::renderer,
// 	                         "close", &Window::Close,
// 	                         "shouldClose", &Window::ShouldClose,
// 	                         "getWidth", &Window::GetWidth,
// 	                         "getHeight", &Window::GetHeight,
// 	                         "getSize", &Window::GetSize);
// };

// void Window::ProvideLuaAPI(ILuaAPI &luaAPI) noexcept
// {
// 	if (auto &&luaAPIProvider = std::dynamic_pointer_cast<Renderer>(renderer); luaAPIProvider != nullptr)
// 	{
// 		luaAPIProvider->ProvideLuaAPI(luaAPI);
// 	}

// 	luaAPI.RegisterInstallation("tudov_Window", windowLuaAPIInstallation);
// }

Context &Window::GetContext() noexcept
{
	return _context;
}

void Window::Initialize(std::int32_t width, std::int32_t height, std::string_view title) noexcept
{
	_sdlWindow = SDL_CreateWindow(title.data(), width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
	if (!_sdlWindow)
	{
		_log->Error("Failed to initialize SDL3 Window");
	}

	renderer->Initialize();
}

bool Window::IsMinimized() const noexcept
{
	return (SDL_GetWindowFlags(_sdlWindow) & SDL_WINDOW_MINIMIZED) != 0;
}

void Window::Close() noexcept
{
	_shouldClose = true;
}

bool Window::ShouldClose() noexcept
{
	return _shouldClose;
}

EventHandleKey Window::GetKey() const noexcept
{
	return {};
}

void Window::HandleEvents() noexcept
{
}

bool Window::HandleEvent(SDL_Event &event) noexcept
{
	if (SDL_GetWindowID(_sdlWindow) != event.window.windowID)
	{
		return false;
	}

	auto &&eventManager = GetEventManager();
	switch (event.type)
	{
	case SDL_EVENT_QUIT:
		Close();
		break;
	case SDL_EVENT_LOW_MEMORY:
		break;
	case SDL_EVENT_MOUSE_MOTION:
		break;
	default:
		break;
	}

	return true;
}

void Window::Render() noexcept
{
	if (RenderPreImpl())
	{
		renderer->Render();
	}
}

bool Window::RenderPreImpl() noexcept
{
	if (IsMinimized())
	{
		return false;
	}

	auto &&args = GetScriptEngine().CreateTable(0, 1);
	auto &&key = GetKey();
	args["isMain"] = GetEngine().GetMainWindow().get() == this;
	args["key"] = &key;
	args["window"] = this;
	GetEventManager().GetCoreEvents().TickRender().Invoke(args, key);

	return true;
}

SDL_Window *Window::GetSDLWindowHandle() noexcept
{
	return _sdlWindow;
}

std::int32_t Window::GetWidth() const noexcept
{
	std::int32_t w;
	SDL_GetWindowSize(_sdlWindow, &w, nullptr);
	return w;
}

std::int32_t Window::GetHeight() const noexcept
{
	std::int32_t h;
	SDL_GetWindowSize(_sdlWindow, nullptr, &h);
	return h;
}

std::tuple<std::int32_t, std::int32_t> Window::GetSize() const noexcept
{
	std::int32_t w, h;
	SDL_GetWindowSize(_sdlWindow, &w, &h);
	return {w, h};
}

std::float_t Window::GetDisplayScale() const noexcept
{
	return SDL_GetDisplayContentScale(SDL_GetDisplayForWindow(_sdlWindow));
}

std::float_t Window::GetGUIScale() const noexcept
{
	std::int32_t w, h;
	SDL_GetWindowSize(_sdlWindow, &w, &h);
	return std::min(w, h) / 720.0f;
}

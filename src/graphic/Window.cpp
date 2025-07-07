#include "Window.h"

#include "Camera2D.h"
#include "Renderer.h"
#include "program/Engine.h"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"
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

Context &Window::GetContext() noexcept
{
	return _context;
}

void Window::Initialize() noexcept
{
	Initialize(800, 600, "Untitled");
}

void Window::Initialize(std::int32_t width, std::int32_t height, std::string_view title) noexcept
{
	_sdlWindow = SDL_CreateWindow(title.data(), width, height, SDL_WINDOW_RESIZABLE);
	if (!_sdlWindow)
	{
		_log->Error("Failed to initialize SDL3 Window");
	}

	renderer->Initialize();
}

void Window::Deinitialize() noexcept
{
	Close();
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
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		HandleEvent(event);
	}
}

void Window::HandleEvent(const SDL_Event &event) noexcept
{
	auto &&eventManager = GetEventManager();

	switch (event.type)
	{
	case SDL_EVENT_QUIT:
		Close();
		break;
	case SDL_EVENT_LOW_MEMORY:
		break;
	}
}

void Window::Render() noexcept
{
	auto &&args = GetScriptEngine()->CreateTable(0, 1);
	args["window"] = this;
	GetEventManager()->GetRenderEvent().Invoke(args, GetKey());
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

std::float_t Window::GetFramerate() const noexcept
{
	return _context.GetEngine().GetFramerate();
}

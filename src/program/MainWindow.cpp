#include "MainWindow.hpp"

#include "Window.hpp"
#include "event/EventHandleKey.hpp"

#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

using namespace tudov;

MainWindow::MainWindow(Context &context) noexcept
    : Window(context, "MainWindow")
{
}

static bool bSDLImGUI = false;

MainWindow::~MainWindow() noexcept
{
	if (bSDLImGUI)
	{
		ImGui_ImplSDLRenderer3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		bSDLImGUI = false;
	}
}

void MainWindow::Initialize(std::int32_t width, std::int32_t height, std::string_view title) noexcept
{
	Window::Initialize(width, height, title);

	if (!bSDLImGUI)
	{
		auto sdlRenderer = renderer->GetSDLRendererHandle();
		ImGui_ImplSDL3_InitForSDLRenderer(_sdlWindow, sdlRenderer);
		ImGui_ImplSDLRenderer3_Init(sdlRenderer);
		bSDLImGUI = true;
	}
}

EventHandleKey MainWindow::GetKey() const noexcept
{
	return {"Main"};
}

void MainWindow::HandleEvents() noexcept
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL3_ProcessEvent(&event);

		HandleEvent(event);
	}
}

void MainWindow::Render() noexcept
{
	Window::Render();

	auto data = ImGui::GetDrawData();
	if (data)
	{
		ImGui_ImplSDLRenderer3_RenderDrawData(data, renderer->GetSDLRendererHandle());
	}

	renderer->Render();
}

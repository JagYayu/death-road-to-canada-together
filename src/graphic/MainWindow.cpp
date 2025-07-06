#include "MainWindow.h"

#include "Window.h"
#include "event/EventHandleKey.hpp"

#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

using namespace tudov;

MainWindow::MainWindow(Engine &engine) noexcept
    : Window(engine, "MainWindow")
{
}

void MainWindow::Initialize(std::int32_t width, std::int32_t height, std::string_view title) noexcept
{
	Window::Initialize(width, height, title);

	auto sdlRenderer = renderer->GetSDLRendererHandle();
	ImGui_ImplSDL3_InitForSDLRenderer(_sdlWindow, sdlRenderer);
	ImGui_ImplSDLRenderer3_Init(sdlRenderer);
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

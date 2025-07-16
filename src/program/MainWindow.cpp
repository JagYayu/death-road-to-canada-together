#include "MainWindow.hpp"

#include "Window.hpp"
#include "debug/DebugManager.hpp"
#include "event/EventHandleKey.hpp"
#include "graphic/RenderTarget.hpp"
#include "program/Engine.hpp"

#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_video.h"
#include "imgui_freetype.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <algorithm>

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

	delete _debugRenderTarget;
}

void MainWindow::Initialize(std::int32_t width, std::int32_t height, std::string_view title) noexcept
{
	Window::Initialize(width, height, title);

	if (!bSDLImGUI)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		{
			ImGuiIO &io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		}

		auto sdlRenderer = renderer->GetSDLRendererHandle();
		ImGui_ImplSDL3_InitForSDLRenderer(_sdlWindow, sdlRenderer);
		ImGui_ImplSDLRenderer3_Init(sdlRenderer);

		ImGuiIO &io = ImGui::GetIO();
		ImFontConfig font_cfg;
		font_cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_NoHinting;
		io.Fonts->AddFontDefault(&font_cfg);
		io.Fonts->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();
		io.Fonts->Build();

		bSDLImGUI = true;
	}

	{
		auto [width, height] = GetSize();
		_debugRenderTarget = new RenderTarget(*renderer, width, height);
	}
}

void MainWindow::SetDebugManager(const std::shared_ptr<IDebugManager> &debugManager) noexcept
{
	_debugManager = debugManager;
}

EventHandleKey MainWindow::GetKey() const noexcept
{
	return {"Main"};
}

bool MainWindow::HandleEvent(SDL_Event &event) noexcept
{
	return Window::HandleEvent(event) && ImGui_ImplSDL3_ProcessEvent(&event);
}

void MainWindow::Render() noexcept
{
	renderer->Clear(SDL_Color{0, 0, 0, 255});

	ImGui_ImplSDL3_NewFrame();
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui::NewFrame();
	{
		auto &&engine = GetEngine();
		if (engine.GetLoadingState() != Engine::ELoadingState::InProgress)
		{
			RenderPreImpl();

			if (!_debugManager.expired())
			{
				_debugManager.lock()->UpdateAndRender(*this);
			}
		}
		else if (engine.IsLoadingLagged())
		{
			RenderLoadingGUI(engine);
		}
	}
	ImGui::EndFrame();
	ImGui::Render();

	{
		auto sdlRenderer = renderer->GetSDLRendererHandle();
		if (auto data = ImGui::GetDrawData(); data)
		{
			ImGui_ImplSDLRenderer3_RenderDrawData(data, sdlRenderer);
		}
	}

	{
		auto [width, height] = GetSize();
		SDL_FRect rect{0, 0, std::float_t(width), std::float_t(height)};
		_debugRenderTarget->Draw(rect);
	}

	renderer->Render();
}

void MainWindow::RenderLoadingGUI(Engine &engine) noexcept
{
	ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImVec2 viewSize = viewport->Size;
	ImGui::SetNextWindowPos(ImVec2(viewSize.x * 0.5f, viewSize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	std::float_t width = viewSize.x;
	std::float_t height = viewSize.y;
	{
		auto size = std::min(viewSize.x, viewSize.y);
		width = size * .5;
		height = size * .2;
	}
	ImGui::SetNextWindowSize(ImVec2(width, height));

	if (ImGui::Begin("Loading", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav))
	{
		auto scale = SDL_GetDisplayContentScale(SDL_GetDisplayForWindow(_sdlWindow));
		auto size = std::max(viewSize.x, viewSize.y);
		auto extraScale = size / 1024;
		// ImGui::SetWindowFontScale(scale);

		auto &&loadingInfo = engine.GetLoadingInfo();

		{
			auto rep = ((engine.GetTick() - engine.GetLoadingBeginTick()) / 200'000'000) % 4;
			ImGui::Text("%s%s", loadingInfo.title.c_str(), std::string(rep, '.').c_str());
			ImGui::Spacing();
		}
		{
			auto visual = loadingInfo.GetVisualProgress();
			ImGui::ProgressBar(visual, ImVec2(width, 0.0f), std::format("{}/{}", loadingInfo.progressValue, loadingInfo.progressTotal).c_str());
			ImGui::Spacing();
		}
		{
			ImGui::TextDisabled("%s", loadingInfo.description.c_str());
		}

		ImGui::End();
	}
}

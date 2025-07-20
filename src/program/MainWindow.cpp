#include "MainWindow.hpp"

#include "Window.hpp"
#include "debug/DebugManager.hpp"
#include "event/EventHandleKey.hpp"
#include "graphic/RenderTarget.hpp"
#include "graphic/Renderer.hpp"
#include "imgui.h"
#include "program/Engine.hpp"
#include "util/StringUtils.hpp"

#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_video.h"
#include "imgui_freetype.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include <algorithm>
#include <memory>
#include <numbers>

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

void SetImGuiStyle(std::float_t scale) noexcept
{
	ImGuiStyle &style = ImGui::GetStyle();

	style.WindowPadding = ImVec2(8 * scale, 8 * scale);
	style.FramePadding = ImVec2(6 * scale, 4 * scale);
	style.CellPadding = ImVec2(4 * scale, 2 * scale);
	style.ItemSpacing = ImVec2(6 * scale, 4 * scale);
	style.ItemInnerSpacing = ImVec2(4 * scale, 4 * scale);
	style.TouchExtraPadding = ImVec2(1 * scale, 1 * scale);
	style.IndentSpacing = 20 * scale;
	style.ScrollbarSize = 12 * scale;
	style.GrabMinSize = 10 * scale;

	// rounding
	style.WindowRounding = 4 * scale;
	style.ChildRounding = 4 * scale;
	style.FrameRounding = 4 * scale;
	style.PopupRounding = 4 * scale;
	style.ScrollbarRounding = 4 * scale;
	style.GrabRounding = 4 * scale;
	style.TabRounding = 4 * scale;

	// align
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Right;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	// colors
	ImVec4 *colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.55f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.14f, 0.17f, 0.80f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.16f, 0.19f, 0.86f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.14f, 0.16f, 0.19f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.20f, 0.23f, 0.28f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.18f, 0.22f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.23f, 0.28f, 0.54f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.22f, 0.26f, 0.32f, 0.54f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.12f, 0.15f, 0.92f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.14f, 0.17f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.12f, 0.15f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.16f, 0.19f, 0.71f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.12f, 0.15f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.35f, 0.42f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.40f, 0.47f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.40f, 0.45f, 0.52f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.40f, 0.65f, 0.95f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.40f, 0.65f, 0.95f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.45f, 0.70f, 1.00f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.40f, 0.65f, 0.95f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.65f, 0.95f, 0.80f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.45f, 0.70f, 1.00f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.40f, 0.65f, 0.95f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.40f, 0.65f, 0.95f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.45f, 0.70f, 1.00f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.30f, 0.35f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.40f, 0.65f, 0.95f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.45f, 0.70f, 1.00f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.40f, 0.65f, 0.95f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.40f, 0.65f, 0.95f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.45f, 0.70f, 1.00f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.16f, 0.19f, 0.86f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.65f, 0.95f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.16f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.12f, 0.15f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.12f, 0.14f, 0.17f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.50f, 0.55f, 0.60f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.40f, 0.65f, 0.95f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.65f, 0.95f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.45f, 0.70f, 1.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.14f, 0.16f, 0.19f, 0.960f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.20f, 0.23f, 0.28f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.16f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.40f, 0.65f, 0.95f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.40f, 0.65f, 0.95f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.40f, 0.65f, 0.95f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.40f, 0.65f, 0.95f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.12f, 0.14f, 0.17f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.12f, 0.14f, 0.17f, 0.35f);
}

void MainWindow::UpdateGuiStyle() noexcept
{
	ImGuiViewport *viewport = ImGui::GetMainViewport();
	if (viewport == nullptr)
	{
		return;
	}

	std::float_t scale = GetGUIScale();
	if (scale <= 0 || _guiScale == scale) [[likely]]
	{
		return;
	}
	_guiScale = scale;

	ImGui_ImplSDL3_Shutdown();
	ImGui_ImplSDLRenderer3_Shutdown();

	SetImGuiStyle(scale);

	ImGuiIO &io = ImGui::GetIO();

	auto &&fonts = io.Fonts;
	fonts->Clear();
	ImFontConfig config;
	config.FontDataOwnedByAtlas = false;
	_guiFontMemory = ReadFileToString("tudov/fonts/FiraCode-Medium.ttf", true);
	_guiFontMedium = fonts->AddFontFromMemoryTTF(_guiFontMemory.data(), _guiFontMemory.size() * sizeof(char), 12.0f * scale, &config);
	_guiFontSmall = fonts->AddFontFromMemoryTTF(_guiFontMemory.data(), _guiFontMemory.size() * sizeof(char), 6.0f * scale, &config);
	_guiFontLarge = fonts->AddFontFromMemoryTTF(_guiFontMemory.data(), _guiFontMemory.size() * sizeof(char), 18.0f * scale, &config);
	fonts->Build();

	auto sdlRenderer = renderer->GetSDLRendererHandle();
	ImGui_ImplSDL3_InitForSDLRenderer(_sdlWindow, sdlRenderer);
	ImGui_ImplSDLRenderer3_Init(sdlRenderer);
}

void MainWindow::Initialize(std::int32_t width, std::int32_t height, std::string_view title) noexcept
{
	Window::Initialize(width, height, title);

	if (!bSDLImGUI)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		{
			ImGuiIO &io = ImGui::GetIO();
			io.IniFilename = nullptr;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		}

		auto sdlRenderer = renderer->GetSDLRendererHandle();
		ImGui_ImplSDL3_InitForSDLRenderer(_sdlWindow, sdlRenderer);
		ImGui_ImplSDLRenderer3_Init(sdlRenderer);

		bSDLImGUI = true;
	}

	// _renderTarget = std::make_shared<RenderTarget>(*renderer, width, height);
	SetShowDebugElements(true);
}

ImFont *MainWindow::GetGUIFontSmall() noexcept
{
	return _guiFontSmall;
}

ImFont *MainWindow::GetGUIFontMedium() noexcept
{
	return _guiFontMedium;
}

ImFont *MainWindow::GetGUIFontLarge() noexcept
{
	return _guiFontLarge;
}

void MainWindow::SetDebugManager(const std::shared_ptr<IDebugManager> &debugManager) noexcept
{
	_debugManager = debugManager;
}

bool MainWindow::GetShowDebugElements() noexcept
{
	return true;
	// return _renderTarget != nullptr;
}

void MainWindow::SetShowDebugElements(bool value) noexcept
{
	// auto [width, height] = GetSize();
	// _renderTarget = std::make_shared<RenderTarget>(*renderer, width, height);
}

bool MainWindow::GetShowLoadingFrame() noexcept
{
	// return _loadingFrameRenderTarget != nullptr;
	return true;
}

void MainWindow::SetShowLoadingFrame(bool value) noexcept
{
	// auto [width, height] = GetSize();
	// _loadingFrameRenderTarget = std::make_shared<RenderTarget>(*renderer, width, height);
}

EventHandleKey MainWindow::GetKey() const noexcept
{
	return "Main";
}

bool MainWindow::HandleEvent(SDL_Event &event) noexcept
{
	return Window::HandleEvent(event) && ImGui_ImplSDL3_ProcessEvent(&event);
}

void MainWindow::Render() noexcept
{
	if (IsMinimized())
	{
		return;
	}

	renderer->Begin();

	UpdateGuiStyle();

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

	// {
	// 	auto [width, height] = GetSize();
	// 	SDL_FRect rect{0, 0, std::float_t(width), std::float_t(height)};
	// 	_renderTarget->Draw(rect);
	// }

	renderer->Render();
	renderer->End();
}

void MainWindow::RenderLoadingGUI(Engine &engine) noexcept
{
	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), ImGui::GetIO().DisplaySize, IM_COL32(0, 0, 0, 128), 0.0f);

	{
		ImGuiViewport *viewport = ImGui::GetMainViewport();
		ImVec2 viewSize = viewport->Size;
		ImGui::SetNextWindowPos(ImVec2(viewSize.x * 0.5f, viewSize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	}
	std::float_t width;
	std::float_t height;
	// {
	// 	auto size = std::min(viewSize.x, viewSize.y);
	// 	width = size * .5;
	// 	height = size * .2;
	// }

	auto scale = GetGUIScale();
	width = 300 * scale;
	height = 100 * scale;
	ImGui::SetNextWindowSize(ImVec2(width, height));

	if (ImGui::Begin("Loading", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav))
	{
		// auto scale = SDL_GetDisplayContentScale(SDL_GetDisplayForWindow(_sdlWindow));
		// auto size = std::max(viewSize.x, viewSize.y);
		// auto extraScale = size / 1024;

		{
			auto radius = 6.0f * scale;
			ImGuiStyle &style = ImGui::GetStyle();
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImVec2 size = ImVec2(radius * scale, radius * scale + style.CellPadding.x);

			ImGui::Dummy(size);

			std::int32_t segments = 24;
			std::float_t start = fabsf(sinf(ImGui::GetTime() * 1.8f) * (segments - 5));
			std::float_t min = std::numbers::pi * 2.0f * start / segments;
			std::float_t max = std::numbers::pi * 2.0f * (segments - 3) / segments;

			ImDrawList *drawList = ImGui::GetWindowDrawList();
			ImVec2 center = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);
			for (auto i = 0; i <= segments; i++)
			{
				float a = min + (i / std::float_t(segments)) * (max - min);
				drawList->PathLineTo(ImVec2(center.x + cosf(a + ImGui::GetTime() * 8) * radius, center.y + sinf(a + ImGui::GetTime() * 8) * radius));
			}
			drawList->PathStroke(IM_COL32(255, 255, 255, 192), false, 3.0f * scale);
		}
		ImGui::SameLine();

		auto &&loadingInfo = engine.GetLoadingInfo();

		ImGui::PushFont(_guiFontLarge);
		{
			auto rep = ((engine.GetTick() - engine.GetLoadingBeginTick()) / 400'000'000) % 4;
			ImGui::Text("%s%s", loadingInfo.title.c_str(), std::string(rep, '.').c_str());
			ImGui::Spacing();
		}
		{
			auto visual = loadingInfo.GetVisualProgress();
			auto w = width - ImGui::GetStyle().WindowPadding.x * 2;
			ImGui::ProgressBar(visual, ImVec2(w, 0.0f),
			                   std::format("{}/{}", loadingInfo.progressValue, loadingInfo.progressTotal).c_str());
			ImGui::Spacing();
		}
		ImGui::PopFont();
		{
			ImGui::TextDisabled("%s", loadingInfo.description.c_str());
		}

		ImGui::End();
	}
}

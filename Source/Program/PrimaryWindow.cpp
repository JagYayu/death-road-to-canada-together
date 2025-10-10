/**
 * @file program/PrimaryWindow.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Program/PrimaryWindow.hpp"

#include "Debug/DebugManager.hpp"
#include "Event/AppEvent.hpp"
#include "Event/EventHandleKey.hpp"
#include "Graphic/RenderTarget.hpp"
#include "Graphic/Renderer.hpp"
#include "mod/ScriptErrors.hpp"
#include "Program/Engine.hpp"
#include "Program/Window.hpp"
#include "System/Time.hpp"
#include "Util/StringUtils.hpp"

#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_video.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include <chrono>
#include <memory>
#include <numbers>

using namespace tudov;

PrimaryWindow::PrimaryWindow(Context &context) noexcept
    : Window(context, "PrimaryWindow"),
      _showDebugElements()
{
}

static bool bSDLImGUI = false;

PrimaryWindow::~PrimaryWindow() noexcept
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

void PrimaryWindow::UpdateGuiStyle() noexcept
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
	_guiFontMemory = ReadFileToString("dev/fonts/JetBrainsMapleMono.ttf", true);
	_guiFontMedium = fonts->AddFontFromMemoryTTF(_guiFontMemory.data(), _guiFontMemory.size() * sizeof(char), 12.0f * scale, &config);
	_guiFontSmall = fonts->AddFontFromMemoryTTF(_guiFontMemory.data(), _guiFontMemory.size() * sizeof(char), 6.0f * scale, &config);
	_guiFontLarge = fonts->AddFontFromMemoryTTF(_guiFontMemory.data(), _guiFontMemory.size() * sizeof(char), 18.0f * scale, &config);
	fonts->Build();

	auto *sdlRenderer = _renderer->GetSDLRendererHandle();
	ImGui_ImplSDL3_InitForSDLRenderer(_sdlWindow, sdlRenderer);
	ImGui_ImplSDLRenderer3_Init(sdlRenderer);
}

void PrimaryWindow::InitializeWindow(std::int32_t width, std::int32_t height, std::string_view title) noexcept
{
	_sdlWindow = SDL_CreateWindow(title.data(), width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
	if (!_sdlWindow)
	{
		_log->Fatal("Failed to initialize SDL3 Window");
	}

	_renderer->InitializeRenderer();

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

		SDL_Renderer *sdlRenderer = _renderer->GetSDLRendererHandle();
		ImGui_ImplSDL3_InitForSDLRenderer(_sdlWindow, sdlRenderer);
		ImGui_ImplSDLRenderer3_Init(sdlRenderer);

		bSDLImGUI = true;
	}

	_renderTarget = std::make_shared<RenderTarget>(*_renderer, width, height);

	SetShowDebugElements(true);
}

void PrimaryWindow::DeinitializeWindow() noexcept
{
	_renderer->DeinitializeRenderer();

	SDL_DestroyWindow(_sdlWindow);
}

ImFont *PrimaryWindow::GetGUIFontSmall() noexcept
{
	return _guiFontSmall;
}

ImFont *PrimaryWindow::GetGUIFontMedium() noexcept
{
	return _guiFontMedium;
}

ImFont *PrimaryWindow::GetGUIFontLarge() noexcept
{
	return _guiFontLarge;
}

void PrimaryWindow::SetDebugManager(const std::shared_ptr<IDebugManager> &debugManager) noexcept
{
	_debugManager = debugManager;
}

bool PrimaryWindow::GetShowDebugElements() noexcept
{
	return _showDebugElements;
}

void PrimaryWindow::SetShowDebugElements(bool value) noexcept
{
	_showDebugElements = value;
}

bool PrimaryWindow::GetShowLoadingFrame() noexcept
{
	return true;
}

void PrimaryWindow::SetShowLoadingFrame(bool value) noexcept
{
}

EventHandleKey PrimaryWindow::GetKey() const noexcept
{
	static EventHandleKey key = EventHandleKey("Primary");
	return key;
}

bool PrimaryWindow::HandleEvent(AppEvent &appEvent) noexcept
{
	return Window::HandleEvent(appEvent) | ImGui_ImplSDL3_ProcessEvent(appEvent.sdlEvent.get());
}

void PrimaryWindow::Render() noexcept
{
	if (IsMinimized())
	{
		return;
	}

	bool showGUIs = true;

	_renderer->Begin();

	if (showGUIs)
	{
		UpdateGuiStyle();

		ImGui_ImplSDL3_NewFrame();
		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui::NewFrame();
	}

	Engine &engine = GetEngine();

	{
		auto &loadingMutex = engine.GetLoadingMutex();

		if (loadingMutex.try_lock_for(std::chrono::milliseconds(10)))
		{
			if (engine.GetLoadingState() == Engine::ELoadingState::Done)
			{
				RenderPreImpl();

				if (!_debugManager.expired())
				{
					_debugManager.lock()->UpdateAndRender(*this);
				}
			}

			loadingMutex.unlock();
		}
	}

	if (showGUIs)
	{
		if (engine.IsLoadingLagged())
		{
			RenderLoadingGUI(engine);
		}

		if (GetScriptErrors().HasLoadtimeError())
		{
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), ImGui::GetIO().DisplaySize, IM_COL32(0, 0, 0, 128), 0.0f);
		}

		ImGui::EndFrame();
		ImGui::Render();
	}

	_renderTarget->Update();
	_renderTarget->ResizeToFit();

	_renderer->BeginTarget(_renderTarget);
	_renderer->Clear();

	if (auto *data = ImGui::GetDrawData(); data != nullptr)
	{
		ImGui_ImplSDLRenderer3_RenderDrawData(data, _renderer->GetSDLRendererHandle());
	}

	_renderer->EndTarget();

	try
	{
		auto [w, h] = GetSize();
		SDL_FRect dst{
		    .x = 0,
		    .y = 0,
		    .w = static_cast<std::float_t>(w),
		    .h = static_cast<std::float_t>(h),
		};
		_renderer->DrawRect(_renderTarget->GetTexture().get(), dst);
	}
	catch (std::exception &e)
	{
	}

	if (GetScriptErrors().HasLoadtimeError())
	{
		auto scriptError = GetScriptErrors().GetLoadtimeErrorsCached().at(0);

		auto channel = Time::GetSystemTime() * 5.0;
		channel = std::floor(256 - 128 * std::sin(channel - std::floor(channel)));

		{
			decltype(auto) text = "Loadtime error occurred!";
			_renderer->DrawDebugText(1, 0, text, Color(255, channel, channel, 255));
		}

		std::istringstream stream{scriptError->message};
		std::string line;
		std::size_t index = 0;
		while (std::getline(stream, line))
		{
			++index;

			_renderer->DrawDebugText(1, 3 + 9 * index, line, Color(255, channel, channel, 255));
		}
	}

	_renderer->End();
}

void PrimaryWindow::RenderLoadingGUI(Engine &engine) noexcept
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

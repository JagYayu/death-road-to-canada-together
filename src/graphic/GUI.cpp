#include "graphic/GUI.hpp"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

using namespace tudov;

void GUI::TryInit() noexcept
{
	if (!_isOn)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		{
			ImGuiIO &io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		}

		_isOn = true;
	}
}

void GUI::Quit() noexcept
{
	if (_isOn)
	{
		ImGui_ImplSDLRenderer3_Shutdown();
		ImGui_ImplSDL3_Shutdown();

		_isOn = false;
	}
}

bool GUI::Attach(SDL_Renderer *sdlRenderer) noexcept
{
	return _sdlRenderers.emplace(sdlRenderer).second;
}

bool GUI::Detach(SDL_Renderer *sdlRenderer) noexcept
{
	return _sdlRenderers.erase(sdlRenderer);
}

std::float_t GUI::GetScale() noexcept
{
	return _scale;
}

void GUI::SetScale(std::float_t scale) noexcept
{
	if (_scale == scale)
	{
		return;
	}

	_scale = scale;
}

// ImFont &GUI::GetSmallFont() noexcept
// {
// }

// ImFont &GUI::GetMediumFont() noexcept
// {
// }

// ImFont &GUI::GetLargeFont() noexcept
// {
// }

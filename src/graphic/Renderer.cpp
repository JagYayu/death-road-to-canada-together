#include "Renderer.h"

#include "Window.h"
#include "program/Engine.h"

#include "SDL3/SDL_render.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include <algorithm>

using namespace tudov;

Renderer::Renderer(Window &window)
    : window(window)
{
}

SDL_Renderer *Renderer::GetRaw() noexcept
{
	return _renderer;
}

const SDL_Renderer *Renderer::GetRaw() const noexcept
{
	return _renderer;
}

void Renderer::Initialize() noexcept
{
	_renderer = SDL_CreateRenderer(window._window, nullptr);
	if (!_renderer)
	{
		SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
	}

	ImGui_ImplSDL3_InitForSDLRenderer(window._window, _renderer);
	ImGui_ImplSDLRenderer3_Init(_renderer);

	auto &&render = window.engine.modManager.scriptEngine.CreateTable();

	// TODO
	render["drawSprite"] = [this](ResourceID texID, tudov::Number x, tudov::Number y, tudov::Number w, tudov::Number h, tudov::Number tx, tudov::Number ty)
	{
		DrawSprite(texID, x, y, w, h, tx, ty);
	};

	window.engine.modManager.scriptEngine.SetReadonlyGlobal("Render", render);
}

void Renderer::DrawSprite(ResourceID texID, Number x, Number y, Number w, Number h, Number tw, Number th)
{
	auto &&texRes = window.engine.textureManager.GetResource(texID);
	if (!texRes)
	{
		return;
	}

	Draw draw;
	draw.tex = texRes->_texture;
	draw.dst = SDL_FRect{(float)x, (float)y, (float)w, (float)h};
	draw.src = SDL_FRect{(float)x, (float)y, (float)w, (float)h};
	_drawQueue.push_back(draw);
}

void Renderer::Begin()
{
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
	SDL_RenderClear(_renderer);
}

void Renderer::End()
{
	std::sort(_drawQueue.begin(), _drawQueue.end(), [](const auto &lhs, const auto &rhs)
	{
		return lhs.tex < rhs.tex;
	});

	SDL_Texture *currentTexture = nullptr;

	for (const auto &draw : _drawQueue)
	{
		auto tex = draw.tex;
		if (tex != currentTexture)
		{
			currentTexture = tex;
			SDL_SetTextureBlendMode(currentTexture, SDL_BLENDMODE_BLEND);
		}
		SDL_RenderTexture(_renderer, tex, &draw.src, &draw.dst);
	}

	_drawQueue.clear();

	auto &&io = ImGui::GetIO();
	auto &&ptr = window.renderer.GetRaw();

	SDL_SetRenderScale(ptr, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
	SDL_SetRenderDrawColorFloat(ptr, 0, 0, 0, 255);
	SDL_RenderClear(ptr);
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), ptr);
	SDL_RenderPresent(ptr);
}

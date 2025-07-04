#include "SDLRenderer.h"

#include "../Window.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "SDLRenderBuffer.h"
#include "SDLTexture.h"
#include "glm/ext/matrix_float3x3.hpp"
#include "graphic/IRenderer.h"

#include "SDL3/SDL_render.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include <memory>

#ifdef DrawText
#undef DrawText
#endif

using namespace tudov;

SDLRenderer::SDLRenderer(Window &window)
    : IRenderer(window),
      _log(Log::Get("SDLRenderer"))
{
	_textEngine = TTF_CreateRendererTextEngine(_renderer);
}

SDL_Renderer *SDLRenderer::GetRaw() noexcept
{
	return _renderer;
}

const SDL_Renderer *SDLRenderer::GetRaw() const noexcept
{
	return _renderer;
}

TTF_TextEngine *SDLRenderer::GetRawTextEngine() noexcept
{
	return _textEngine;
}

const TTF_TextEngine *SDLRenderer::GetRawTextEngine() const noexcept
{
	return _textEngine;
}

// ERenderBackend SDLRenderer::GetRenderBackend() const noexcept
// {
// 	return ERenderBackend::SDL;
// }

void SDLRenderer::Initialize() noexcept
{
	_renderer = SDL_CreateRenderer(window._window, nullptr);
	if (!_renderer)
	{
		SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
	}

	ImGui_ImplSDL3_InitForSDLRenderer(window._window, _renderer);
	ImGui_ImplSDLRenderer3_Init(_renderer);
}

// void SDLRenderer::InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept
// {
// 	auto &&render = scriptEngine.CreateTable();

// 	// render["draw"] = [&](ResourceID texID, float x, float y, float w, float h, float tx, float ty, float tw, float th, float ang, float cx, float cy, uint32_t flip)
// 	// {
// 	// 	try
// 	// 	{
// 	// 		Draw(texID, x, y, w, h, tx, ty, tw, th, ang, cx, cy, flip);
// 	// 	}
// 	// 	catch (std::exception &e)
// 	// 	{
// 	// 		scriptEngine.ThrowError(std::format("C++ exception: {}", e.what()));
// 	// 	}
// 	// };

// 	auto &&lua = scriptEngine.GetState();

// 	lua.new_usertype<IRenderBuffer>("IRenderBuffer", sol::no_constructor,
// 	                                "clear", &IRenderBuffer::Clear,
// 	                                "draw", &IRenderBuffer::LuaDraw,
// 	                                "drawText", &IRenderBuffer::LuaDrawText,
// 	                                "getTransform", &IRenderBuffer::LuaGetTransform,
// 	                                "render", &IRenderBuffer::Render,
// 	                                "setTransform", &IRenderBuffer::LuaSetTransform,
// 	                                "sort", &IRenderBuffer::Sort);
// 	lua.new_usertype<SDLRenderBuffer>("SDLRenderBuffer", sol::base_classes, sol::bases<IRenderBuffer>());

// 	render["newBuffer"] = [&]()
// 	{
// 		return std::make_shared<SDLRenderBuffer>(*this);
// 	};

// 	scriptEngine.SetReadonlyGlobal("Render", render);
// }

// void SDLRenderer::Draw(ResourceID texID, float x, float y, float w, float h, float tx, float ty, float tw, float th, float ang, float cx, float cy, uint32_t flip)
// {
// 	auto &&texRes = window.engine.textureManager.GetResource(texID);
// 	if (!texRes)
// 	{
// 		auto &&msg = std::format("Texture not found: <{}>", texID);
// 		throw std::exception(msg.c_str());
// 	}

// 	auto &&tex = std::dynamic_pointer_cast<SDLTexture>(texRes);
// 	if (!tex)
// 	{
// 		auto &&msg = std::format("Invalid texture type");
// 		throw std::exception(msg.c_str());
// 	}

// 	_commandQueue.push_back(Command{
// 	    .tex = tex->GetRaw(),
// 	    .dst = SDL_FRect{(float)x, (float)y, (float)w, (float)h},
// 	    .src = SDL_FRect{(float)x, (float)y, (float)w, (float)h},
// 	    .ang = ang,
// 	    .ctr = SDL_FPoint{cx, cy},
// 	    .flip = (SDL_FlipMode)flip,
// 	});
// }

void SDLRenderer::Begin()
{
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
	SDL_RenderClear(_renderer);
}

void SDLRenderer::End()
{
	// std::sort(_commandQueue.begin(), _commandQueue.end(), [](const auto &lhs, const auto &rhs)
	// {
	// 	return lhs.tex < rhs.tex;
	// });

	// SDL_Texture *currentTexture = nullptr;

	// for (const auto &cmd : _commandQueue)
	// {
	// 	SDL_RenderTextureRotated(_renderer, cmd.tex, &cmd.src, &cmd.dst, cmd.ang, &cmd.ctr, cmd.flip);
	// }

	// _commandQueue.clear();

	SDL_RenderPresent(_renderer);
}

glm::mat3x3 SDLRenderer::GetTransform()
{
	return glm::mat3x3();
}

void SDLRenderer::SetTransform(const glm::mat3x3 &mat3)
{
}

void SDLRenderer::Clear()
{
}

void SDLRenderer::Draw(const DrawArgs &args)
{
}

SDL_FRect SDLRenderer::DrawText(const DrawTextArgs &args)
{
	return {};
}

void SDLRenderer::Sort()
{
}

void SDLRenderer::Render()
{
}

sol::object SDLRenderer::LuaGetTransform()
{
	return {};
}

void SDLRenderer::LuaSetTransform(const sol::table &mat3)
{
	// SDL_RenderGeometryRaw(SDL_Renderer *renderer, SDL_Texture *texture, const float *xy, int xy_stride, const SDL_FColor *color, int color_stride, const float *uv, int uv_stride, int num_vertices, const void *indices, int num_indices, int size_indices)
	// SDL_SetRenderScale(_renderer, float scaleX, float scaleY)
}

#include "SDLTexture.h"

#include "../IRenderer.h"
#include "../ITexture.h"
#include "../Window.h"
#include "SDLRenderer.h"

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDLSurface.h"
#include "graphic/ERenderBackend.h"
#include "graphic/ITexture.h"
#include <SDL3_image/SDL_image.h>

#include <cassert>
#include <exception>
#include <memory>

using namespace tudov;

SDLTexture::SDLTexture(SDLRenderer &renderer, const std::shared_ptr<SDLSurface> &surface)
    : ITexture(renderer)
{
	this->surface = std::dynamic_pointer_cast<ISurface>(surface);

	auto rawRenderer = renderer.GetRaw();
	auto rawSurface = surface->GetRaw();
	_texture = SDL_CreateTextureFromSurface(rawRenderer, rawSurface);

	if (!_texture)
	{
		auto &&msg = std::format("Failed to load texture: \"{}\"", std::string_view(SDL_GetError()));
		throw std::exception(msg.c_str());
	}
}

SDLTexture::~SDLTexture() noexcept
{
	if (_texture)
	{
		SDL_DestroyTexture(_texture);
		_texture = nullptr;
	}
}

SDL_Texture *SDLTexture::GetRaw() noexcept
{
	return _texture;
}

const SDL_Texture *SDLTexture::GetRaw() const noexcept
{
	return _texture;
}

ERenderBackend SDLTexture::GetRenderBackend() const noexcept
{
	return ERenderBackend::SDL;
}

std::optional<std::reference_wrapper<SDLSurface>> SDLTexture::GetSurface() noexcept
{
	if (surface)
	{
		return static_cast<SDLSurface &>(*surface);
	}
	return std::nullopt;
}

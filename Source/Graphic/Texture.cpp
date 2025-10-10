/**
 * @file graphic/Texture.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Graphic/Texture.hpp"

#include "Graphic/Image.hpp"
#include "Graphic/Renderer.hpp"
#include "Util/Micros.hpp"

#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "imgui.h"
#include "stb_image.h"

#include <stdexcept>

using namespace tudov;

Texture::Texture(Renderer &renderer) noexcept
    : renderer(renderer),
      _sdlTexture(nullptr)
{
}

Texture::~Texture() noexcept
{
	if (_sdlTexture != nullptr)
	{
		SDL_DestroyTexture(_sdlTexture);
	}
}

TE_FORCEINLINE void AssertUninitialized(SDL_Texture *sdlTexture)
{
	if (sdlTexture != nullptr) [[unlikely]]
	{
		throw std::runtime_error("Attempt to reinitialize texture");
	}
}

TE_FORCEINLINE void AssertInitialized(SDL_Texture *sdlTexture)
{
	if (sdlTexture == nullptr) [[unlikely]]
	{
		throw std::runtime_error("Texture_depreciated was not initialized");
	}
}

void Texture::Initialize(std::int32_t width, std::int32_t height, SDL_PixelFormat format, SDL_TextureAccess access)
{
	AssertUninitialized(_sdlTexture);

	_sdlTexture = SDL_CreateTexture(renderer.GetSDLRendererHandle(), format, access, width, height);

	PostInitialize();
}

void Texture::Initialize(Image &image)
{
	AssertUninitialized(_sdlTexture);

	auto sdlSurface = static_cast<SDL_Surface *>(image.GetSDLSurfaceHandle());
	_sdlTexture = SDL_CreateTextureFromSurface(renderer.GetSDLRendererHandle(), sdlSurface);

	PostInitialize();
}

bool LoadTextureFromMemory(const void *data, size_t data_size, SDL_Renderer *renderer, SDL_Texture **out_texture, int *out_width, int *out_height)
{
	int image_width = 0;
	int image_height = 0;
	int channels = 4;
	unsigned char *image_data = stbi_load_from_memory((const unsigned char *)data, (int)data_size, &image_width, &image_height, NULL, 4);
	if (image_data == nullptr)
	{
		fprintf(stderr, "Failed to load image: %s\n", stbi_failure_reason());
		return false;
	}

	SDL_Surface *surface = SDL_CreateSurfaceFrom(image_width, image_height, SDL_PIXELFORMAT_RGBA32, (void *)image_data, channels * image_width);
	if (surface == nullptr)
	{
		fprintf(stderr, "Failed to create SDL surface: %s\n", SDL_GetError());
		return false;
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == nullptr)
		fprintf(stderr, "Failed to create SDL texture: %s\n", SDL_GetError());

	*out_texture = texture;
	*out_width = image_width;
	*out_height = image_height;

	SDL_DestroySurface(surface);
	stbi_image_free(image_data);

	return true;
}

// Open and read a file, then forward to LoadTextureFromMemory()
bool LoadTextureFromFile(const char *file_name, SDL_Renderer *renderer, SDL_Texture **out_texture, int *out_width, int *out_height)
{
	FILE *f = fopen(file_name, "rb");
	if (f == NULL)
		return false;
	fseek(f, 0, SEEK_END);
	size_t file_size = (size_t)ftell(f);
	if (file_size == -1)
		return false;
	fseek(f, 0, SEEK_SET);
	void *file_data = IM_ALLOC(file_size);
	fread(file_data, 1, file_size, f);
	fclose(f);
	bool ret = LoadTextureFromMemory(file_data, file_size, renderer, out_texture, out_width, out_height);
	IM_FREE(file_data);
	return ret;
}

void Texture::InitializeImGUI(std::string_view fileName)
{
	LoadTextureFromFile(fileName.data(), renderer.GetSDLRendererHandle(), &_sdlTexture, nullptr, nullptr);
}

void Texture::PostInitialize() noexcept
{
	SDL_SetTextureScaleMode(_sdlTexture, SDL_SCALEMODE_NEAREST);
	SDL_SetTextureBlendMode(_sdlTexture, SDL_BLENDMODE_BLEND);
}

std::float_t Texture::GetWidth() const
{
	AssertInitialized(_sdlTexture);

	std::float_t width;
	SDL_GetTextureSize(_sdlTexture, &width, nullptr);
	return width;
}

std::float_t Texture::GetHeight() const
{
	AssertInitialized(_sdlTexture);

	std::float_t height;
	SDL_GetTextureSize(_sdlTexture, nullptr, &height);
	return height;
}

std::tuple<std::float_t, std::float_t> Texture::GetSize() const
{
	AssertInitialized(_sdlTexture);

	std::float_t width, height;
	SDL_GetTextureSize(_sdlTexture, &width, &height);
	return {width, height};
}

SDL_Texture *Texture::GetSDLTextureHandle() noexcept
{
	return _sdlTexture;
}

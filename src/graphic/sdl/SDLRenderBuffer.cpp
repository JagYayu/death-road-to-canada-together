#include "SDLRenderBuffer.h"

#include "SDL3/SDL_surface.h"
#include "SDLRenderer.h"
#include "SDLTexture.h"
#include "glm/ext/matrix_float3x3.hpp"
#include "graphic/Window.h"
#include "program/Engine.h"
#include "sol/types.hpp"
#include "util/Utils.hpp"

#include <cmath>
#include <exception>

using namespace tudov;

SDLRenderBuffer::SDLRenderBuffer(SDLRenderer &renderer) noexcept
    : IRenderBuffer(renderer),
      _log(Log::Get("SDLRenderBuffer")),
      _transform()
{
}

glm::mat3x3 &SDLRenderBuffer::GetTransform() noexcept
{
	return _transform;
}

void SDLRenderBuffer::SetTransform(const glm::mat3x3 &transform) noexcept
{
	_transform = transform;
}

sol::object SDLRenderBuffer::LuaGetTransform() noexcept
{
	try
	{
		auto &&tbl = renderer.window.engine.modManager.scriptEngine.CreateTable();
		tbl[1] = _transform[0][0];
		tbl[2] = _transform[1][0];
		tbl[3] = _transform[2][0];
		tbl[4] = _transform[0][1];
		tbl[5] = _transform[1][1];
		tbl[6] = _transform[2][1];
		tbl[7] = _transform[0][2];
		tbl[8] = _transform[1][2];
		tbl[9] = _transform[2][2];
		return tbl;
	}
	catch (std::exception &e)
	{
		UnhandledCppException(_log, e);
		return sol::nil;
	}
}

void SDLRenderBuffer::LuaSetTransform(const sol::table &mat3) noexcept
{
	try
	{
		_transform = glm::mat3{
		    mat3.get<float>(1),
		    mat3.get<float>(4),
		    mat3.get<float>(7),
		    mat3.get<float>(2),
		    mat3.get<float>(5),
		    mat3.get<float>(8),
		    mat3.get<float>(3),
		    mat3.get<float>(6),
		    mat3.get<float>(9),
		};
	}
	catch (std::exception &e)
	{
		UnhandledCppException(_log, e);
	}
}

void SDLRenderBuffer::Clear() noexcept
{
	_commandQueue.clear();
}

void SDLRenderBuffer::Draw(float x, float y, float w, float h, ResourceID t, float tx, float ty, float tw, float th, float a, float cx, float cy, float z) noexcept
{
	try
	{
		auto &&texRes = renderer.window.engine.textureManager.GetResource(t);
		if (!texRes)
		{
			auto &&msg = std::format("Texture not found: <{}>", t);
			throw std::exception(msg.c_str());
		}

		auto &&tex = std::dynamic_pointer_cast<SDLTexture>(texRes);
		if (!tex)
		{
			auto &&msg = std::format("Invalid texture type");
			throw std::exception(msg.c_str());
		}

		glm::vec3 tl = _transform * glm::vec3(x, y, 1.0f);
		glm::vec3 tr = _transform * glm::vec3(x + w, y, 1.0f);
		glm::vec3 bl = _transform * glm::vec3(x, y + h, 1.0f);
		glm::vec3 br = _transform * glm::vec3(x + w, y + h, 1.0f);
		float minX = std::min({tl.x, tr.x, bl.x, br.x});
		float minY = std::min({tl.y, tr.y, bl.y, br.y});
		float maxX = std::max({tl.x, tr.x, bl.x, br.x});
		float maxY = std::max({tl.y, tr.y, bl.y, br.y});
		float newW = maxX - minX;
		float newH = maxY - minY;
		glm::vec3 transformedCtr = _transform * glm::vec3(cx, cy, 1.0f);
		SDL_FPoint relativeCtr = SDL_FPoint{transformedCtr.x - minX, transformedCtr.y - minY};

		_commandQueue.push_back(Command{
		    .tex = tex->GetRaw(),
		    .dst = SDL_FRect{minX, minY, newW, newH},
		    .src = SDL_FRect{tx, ty, tw, th},
		    .ang = a,
		    .ctr = relativeCtr,
		});
	}
	catch (std::exception &e)
	{
		_log->Error("C++ exception: {}", e.what());
	}
}

void SDLRenderBuffer::Sort() noexcept
{
	std::sort(_commandQueue.begin(), _commandQueue.end(), [](const auto &lhs, const auto &rhs)
	{
		return lhs.tex < rhs.tex;
	});

	SDL_Texture *currentTexture = nullptr;
}

void SDLRenderBuffer::Render() noexcept
{
	auto &&sdlRenderer = static_cast<SDLRenderer &>(renderer);

	for (const auto &cmd : _commandQueue)
	{
		SDL_RenderTextureRotated(sdlRenderer.GetRaw(), cmd.tex, &cmd.src, &cmd.dst, cmd.ang, &cmd.ctr, SDL_FLIP_NONE);
	}
}

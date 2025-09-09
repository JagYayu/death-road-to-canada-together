/**
 * @file graphic/RenderBuffer.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "graphic/RenderBuffer.hpp"
#include "graphic/Renderer.hpp"

#include "sol/table.hpp"

#include <memory>

using namespace tudov;

void RenderBuffer::Batch::AddRectangle(RectangleF rectangle, Color color) noexcept
{
	// 0 1
	// 3 2

	std::int32_t index = vertices.size();
	auto col = static_cast<SDL_FColor>(color);
	SDL_FPoint uv{};

	vertices.emplace_back(SDL_FPoint(rectangle.x, rectangle.y), col, uv);
	vertices.emplace_back(SDL_FPoint(rectangle.x + rectangle.w, rectangle.y), col, uv);
	vertices.emplace_back(SDL_FPoint(rectangle.x + rectangle.w, rectangle.y + rectangle.h), col, uv);
	vertices.emplace_back(SDL_FPoint(rectangle.x, rectangle.y + rectangle.h), col, uv);

	indices.push_back(index + 0);
	indices.push_back(index + 1);
	indices.push_back(index + 2);
	indices.push_back(index + 0);
	indices.push_back(index + 2);
	indices.push_back(index + 3);
}

void RenderBuffer::Batch::Clear() noexcept
{
	vertices.clear();
	indices.clear();
}

void RenderBuffer::Batch::Draw(Renderer &renderer) const
{
	renderer.DrawVertices(nullptr, vertices, indices);
}

void RenderBuffer::BatchWithTexture::AddRectangle(RectangleF rectangle, RectangleF source, Color color) noexcept
{
	if (auto tex = texture.lock(); tex != nullptr) [[likely]]
	{
		std::int32_t index = vertices.size();
		auto col = static_cast<SDL_FColor>(color);
		auto [texW, texH] = tex->GetSize();

		vertices.emplace_back(SDL_FPoint(rectangle.x, rectangle.y), col, SDL_FPoint(source.x / texW, source.y / texH));
		vertices.emplace_back(SDL_FPoint(rectangle.x + rectangle.w, rectangle.y), col, SDL_FPoint((source.x + source.w) / texW, source.y / texH));
		vertices.emplace_back(SDL_FPoint(rectangle.x + rectangle.w, rectangle.y + rectangle.h), col, SDL_FPoint((source.x + source.w) / texW, (source.y + source.h) / texH));
		vertices.emplace_back(SDL_FPoint(rectangle.x, rectangle.y + rectangle.h), col, SDL_FPoint(source.x / texW, (source.y + source.h) / texH));

		indices.push_back(index + 0);
		indices.push_back(index + 2);
		indices.push_back(index + 1);
		indices.push_back(index + 0);
		indices.push_back(index + 3);
		indices.push_back(index + 2);
	}
	else [[unlikely]]
	{
		Batch::AddRectangle(rectangle, color);
	}
}

void RenderBuffer::BatchWithTexture::Draw(Renderer &renderer) const
{
	if (!texture.expired()) [[likely]]
	{
		renderer.DrawVertices(texture.lock().get(), vertices, indices);
	}
}

RenderBuffer::RenderBuffer(Renderer &renderer) noexcept
    : renderer(renderer),
      _batch(),
      _batchesWithTexture()
{
}

RenderBuffer::~RenderBuffer() noexcept
{
}

Context &RenderBuffer::GetContext() noexcept
{
	return renderer.GetContext();
}

void RenderBuffer::AddRectangle(RectangleF rectangle, Color color) noexcept
{
	_batch.AddRectangle(rectangle, color);
}

void RenderBuffer::AddRectangle(const std::shared_ptr<Texture> &texture, RectangleF rectangle, RectangleF source, Color color) noexcept
{
	auto it = _batchesWithTexture.find(texture.get());
	if (it == _batchesWithTexture.end()) [[unlikely]]
	{
		_batchesWithTexture[texture.get()] = BatchWithTexture{
		    .texture = texture,
		};
		it = _batchesWithTexture.find(texture.get());
	}

	it->second.AddRectangle(rectangle, source, color);
}

void RenderBuffer::Draw()
{
	_batch.Draw(renderer);

	for (auto &&[_, batch] : _batchesWithTexture)
	{
		batch.Draw(renderer);
	}
}

void RenderBuffer::Clear() noexcept
{
	_batch.Clear();

	for (auto it = _batchesWithTexture.begin(); it != _batchesWithTexture.end();)
	{
		auto &&batch = it->second;
		if (batch.texture.expired()) [[unlikely]]
		{
			it = _batchesWithTexture.erase(it);
		}
		else
		{
			batch.Clear();
			++it;
		}
	}
}

void RenderBuffer::LuaAddRectangle(sol::table args) noexcept
{
	try
	{
		auto dst = args.get<sol::table>("destination");
		if (!dst.valid()) [[unlikely]]
		{
			GetScriptEngine().ThrowError("Bad argument #1 to 'args': missing table field 'destination'");
		}

		RectangleF dstRect{
		    dst.get_or(1, 0.0f),
		    dst.get_or(2, 0.0f),
		    dst.get_or(3, 0.0f),
		    dst.get_or(4, 0.0f),
		};
		Color col{args.get_or("color", -1ui32)};

		std::shared_ptr<Texture> texture = renderer.ExtractTexture(args);

		if (texture != nullptr)
		{
			auto src = args.get<sol::table>("source");
			RectangleF srcRect{
			    src.get_or(1, 0.0f),
			    src.get_or(2, 0.0f),
			    src.get_or(3, 0.0f),
			    src.get_or(4, 0.0f),
			};

			AddRectangle(texture, dstRect, srcRect, col);
		}
		else
		{
			AddRectangle(dstRect, col);
		}
	}
	catch (std::exception &e)
	{
		GetScriptEngine().ThrowError("C++ exception: {}", e.what());
	}
}

void RenderBuffer::LuaDraw() noexcept
{
	try
	{
		Draw();
	}
	catch (std::exception &e)
	{
		GetScriptEngine().ThrowError("C++ exception: {}", e.what());
	}
}

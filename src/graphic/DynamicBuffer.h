#pragma once

#include "RenderBuffer.h"
#include "SDL3/SDL_pixels.h"
#include "View.h"
#include "bgfx/bgfx.h"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "util/Defs.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace tudov
{
	class Renderer;

	class DynamicBuffer : public RenderBuffer
	{
	  public:
		struct Vertex
		{
			glm::vec3 pos;
			glm::vec2 uv;
			SDL_Color col;
		};

		struct Quad
		{
			std::array<Vertex, 4> verts;
			std::array<uint16_t, 6> indices;
		};

	  private:
		bgfx::DynamicVertexBufferHandle _vertexBuffer;
		bgfx::DynamicIndexBufferHandle _indexBuffer;
		bool _buffersInitialized;
		bgfx::VertexLayout _layout;
		std::unordered_map<TextureID, std::vector<Quad>> _batches;

	  public:
		Renderer &renderer;

	  public:
		explicit DynamicBuffer(Renderer &renderer) noexcept;
		~DynamicBuffer() noexcept;

		void Clear() noexcept;
		void Draw2D(const Draw2DArgs &args) noexcept;
		void Render() noexcept;
	};
} // namespace tudov

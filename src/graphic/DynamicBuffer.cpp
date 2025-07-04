#include "DynamicBuffer.h"

#include "Renderer.h"
#include "Texture2D.h"
#include "Window.h"
#include "program/Engine.h"

#include "bgfx/bgfx.h"
#include "glm/ext/vector_float3.hpp"

using namespace tudov;

DynamicBuffer::DynamicBuffer(Renderer &renderer) noexcept
    : renderer(renderer),
      _buffersInitialized(),
      _layout()
{
	_layout.begin()
	    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
	    .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
	    .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
	    .end();
}

DynamicBuffer::~DynamicBuffer() noexcept
{
	bgfx::destroy(_vertexBuffer);
	bgfx::destroy(_indexBuffer);
}

void DynamicBuffer::Clear() noexcept
{
	_batches.clear();
}

void DynamicBuffer::Draw2D(const Draw2DArgs &args) noexcept
{
	Quad quad;

	// 获取目标矩形的四个角点（未旋转前）
	const float x0 = args.dst.x;
	const float y0 = args.dst.y;
	const float x1 = args.dst.x + args.dst.w;
	const float y1 = args.dst.y + args.dst.h;

	// 设置未旋转前的顶点位置
	glm::vec2 corners[4] = {
	    {x0, y0}, // 左上
	    {x1, y0}, // 右上
	    {x1, y1}, // 右下
	    {x0, y1}  // 左下
	};

	// 如果有旋转角度，应用旋转
	if (args.angle != 0.0f)
	{
		const float sin = std::sin(args.angle);
		const float cos = std::cos(args.angle);
		const glm::vec2 center = args.center;

		for (auto &corner : corners)
		{
			// 平移至原点
			corner -= center;
			// 旋转
			glm::vec2 rotated = {
			    corner.x * cos - corner.y * sin,
			    corner.x * sin + corner.y * cos};
			// 平移回原位置
			corner = rotated + center;
		}
	}

	// 设置顶点位置和Z值
	for (int i = 0; i < 4; ++i)
	{
		quad.verts[i].pos = {corners[i].x, corners[i].y, args.z};
	}

	// 设置UV坐标（从SDL_Rect src计算）
	float u0, v0, u1, v1;
	auto &&texture = renderer.window.engine.texture2DManager.GetResource(args.tex);
	if (texture)
	{
		auto [w, h] = texture->GetSize();
		u0 = static_cast<float>(args.src.x) / w;
		v0 = static_cast<float>(args.src.y) / h;
		u1 = static_cast<float>(args.src.x + args.src.w) / w;
		v1 = static_cast<float>(args.src.y + args.src.h) / h;
	}
	else
	{
		u0 = 0;
		v0 = 0;
		u1 = 1;
		v1 = 1;
	}

	quad.verts[0].uv = {u0, v0}; // 左上
	quad.verts[1].uv = {u1, v0}; // 右上
	quad.verts[2].uv = {u1, v1}; // 右下
	quad.verts[3].uv = {u0, v1}; // 左下

	// 设置顶点颜色
	for (auto &vertex : quad.verts)
	{
		vertex.col = args.color;
	}

	// 设置索引（两个三角形组成四边形）
	quad.indices = {0, 1, 2, 0, 2, 3};

	// 将四边形添加到对应的纹理批次中
	_batches[args.tex].push_back(quad);
}

void DynamicBuffer::Render() noexcept
{
	for (auto &[textureID, quads] : _batches)
	{
		std::vector<Vertex> verts;
		std::vector<uint16_t> indices;
		uint16_t base = 0;

		for (auto &quad : quads)
		{
			verts.insert(verts.end(), quad.verts.begin(), quad.verts.end());
			for (auto idx : quad.indices)
			{
				indices.push_back(idx + base);
			}
			base += 4;
		}

		const bgfx::Memory *verticesMem = bgfx::copy(verts.data(), sizeof(Vertex) * verts.size());
		const bgfx::Memory *indicesMem = bgfx::copy(indices.data(), sizeof(uint16_t) * indices.size());

		if (_buffersInitialized)
		{
			bgfx::update(_vertexBuffer, 0, verticesMem);
			bgfx::update(_indexBuffer, 0, indicesMem);
		}
		else
		{
			_vertexBuffer = bgfx::createDynamicVertexBuffer(bgfx::copy(verts.data(), sizeof(Vertex) * verts.size()), _layout);
			_indexBuffer = bgfx::createDynamicIndexBuffer(bgfx::copy(indices.data(), sizeof(uint16_t) * indices.size()));
			_buffersInitialized = true;
		}

		bgfx::setVertexBuffer(0, _vertexBuffer);
		bgfx::setIndexBuffer(_indexBuffer);

		auto &&engine = renderer.window.engine;

		auto &&uniform = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);
		auto &&texture = engine.texture2DManager.GetResource(textureID);
		if (texture)
		{
			bgfx::setTexture(0, uniform, texture->GetHandle());
		}

		renderer.Submit();
	}
}

/**
 * @file graphic/RenderBuffer.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "graphic/Vertex.hpp"
#include "math/Geometry.hpp"
#include "program/Context.hpp"
#include "sol/forward.hpp"
#include "util/Color.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace tudov
{
	class LuaAPI;
	class Renderer;
	class Texture;

	class RenderBuffer : public IContextProvider
	{
		friend LuaAPI;

	  private:
		struct Batch
		{
			std::vector<Vertex> vertices;
			std::vector<std::int32_t> indices;

			void AddRectangle(RectangleF rectangle, Color color) noexcept;
			void Clear() noexcept;
			void Draw(Renderer &renderer) const;
		};

		struct BatchWithTexture : Batch
		{
			std::weak_ptr<Texture> texture;

			void AddRectangle(RectangleF rectangle, RectangleF source, Color color) noexcept;
			void Draw(Renderer &renderer) const;
		};

	  public:
		Renderer &renderer;

	  private:
		Batch _batch;
		std::unordered_map<Texture *, BatchWithTexture> _batchesWithTexture;

	  public:
		explicit RenderBuffer(Renderer &renderer) noexcept;
		~RenderBuffer() noexcept;

		Context &GetContext() noexcept override;

		void AddRectangle(RectangleF rectangle, Color color) noexcept;
		void AddRectangle(const std::shared_ptr<Texture> &texture, RectangleF rectangle, RectangleF source, Color color) noexcept;
		void AddVertices() noexcept;
		void Clear() noexcept;
		void Draw();

	  private:
		void LuaAddRectangle(sol::table args) noexcept;
		void LuaDraw() noexcept;
	};
} // namespace tudov

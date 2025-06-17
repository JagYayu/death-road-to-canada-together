#pragma once

#include "ITexture.h"
#include "util/Defs.h"

#include "glm/glm.hpp"
#include "sol/table.hpp"

#include <array>

namespace tudov
{
	class IRenderer;

	struct IRenderBuffer
	{
		IRenderer &renderer;

		IRenderBuffer(IRenderer &renderer) noexcept;

		virtual glm::mat3x3 &GetTransform() = 0;
		virtual void SetTransform(const glm::mat3x3 &mat3) = 0;
		virtual sol::object LuaGetTransform() = 0;
		virtual void LuaSetTransform(const sol::table &mat3) = 0;
		virtual void Clear() = 0;
		virtual void Draw(float x, float y, float w, float h, ResourceID t, float tx, float ty, float tw, float th, float a, float cx, float cy, float z) noexcept = 0;
		virtual void Sort() = 0;
		virtual void Render() = 0;
	};
} // namespace tudov

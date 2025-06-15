#pragma once

#include "Texture.h"
#include "util/Defs.h"
#include <array>

namespace tudov
{
	struct IRenderBuffer
	{
		virtual std::array<float, 9> GetTransform();
		virtual void SetTransform(const std::array<float, 9> &mat3) = 0;

		virtual void Clear() = 0;
		virtual void Draw(float x, float y, float w, float h, ResourceID t, float tx, float ty, float tw, float th, float a, float cx, float cy, float z) = 0;
		virtual void Sort() = 0;
		virtual void Render() = 0;
	};
} // namespace tudov

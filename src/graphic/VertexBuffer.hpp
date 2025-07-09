#pragma once

#include "rlgl.h"

#include <cstdint>

namespace tudov
{
	class VertexBuffer
	{
	  private:
		::rlVertexBuffer _vertexBuffer;
		std::uint32_t _verticesCount;

	  public:
		explicit VertexBuffer() noexcept;

		void Render() noexcept;
	};
} // namespace tudov

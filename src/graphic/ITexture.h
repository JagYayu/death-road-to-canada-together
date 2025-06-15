#pragma once

namespace tudov
{
	struct IRenderer;

	struct ITexture
	{
		IRenderer &renderer;

		explicit ITexture(IRenderer &renderer) noexcept;
		
		virtual void Test(){};
	};
} // namespace tudov

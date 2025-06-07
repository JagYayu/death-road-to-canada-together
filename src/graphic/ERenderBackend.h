#pragma once

#include <bgfx/bgfx.h>

namespace tudov
{
	enum class ERenderBackend
	{
		None = bgfx::RendererType::Noop,
		Agc = bgfx::RendererType::Agc,
		Direct3D11 = bgfx::RendererType::Direct3D11,
		Direct3D12 = bgfx::RendererType::Direct3D12,
		Gnm = bgfx::RendererType::Gnm,
		Metal = bgfx::RendererType::Metal,
		Nvn = bgfx::RendererType::Nvn,
		OpenGLES = bgfx::RendererType::OpenGLES,
		OpenGL = bgfx::RendererType::OpenGL,
		Vulkan = bgfx::RendererType::Vulkan,
	};
}
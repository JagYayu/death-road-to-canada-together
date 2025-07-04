#pragma once

#include "SDL3/SDL_rect.h"
#include "bgfx/bgfx.h"

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace tudov
{
	class View
	{
	  private:
		static bgfx::ViewId _nextID;
		static std::unordered_map<std::string, std::shared_ptr<View>> _viewMap;

	  public:
		static std::weak_ptr<View> GetView(std::string_view name) noexcept;
		static std::string_view CreateView(std::string_view name = "", SDL_Rect rectangle = {}, uint32_t clearFlags = 0, uint32_t color = 0x00000000, float depth = 1.0f, unsigned char stencil = 0);
		static bool RemoveView(std::string_view name) noexcept;

	  private:
		bgfx::ViewId _id;

	  public:
		explicit View(bgfx::ViewId id) noexcept;
		~View() noexcept;

	  public:
		void SetRect(std::uint16_t x, std::uint16_t y, std::uint16_t w, std::uint16_t h);
		void SetClear(std::uint16_t flags = BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, uint32_t color = 0x000000ff);
		void Touch();
		void Reset();
		void Submit(const bgfx::VertexBufferHandle &vbo, const bgfx::ProgramHandle &program, const float *transform = nullptr);
		bgfx::ViewId GetID() const noexcept;
	};
} // namespace tudov
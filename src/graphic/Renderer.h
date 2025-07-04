#pragma once

#include "DynamicBuffer.h"
#include "IRenderer.h"
#include "View.h"


#include "bgfx/defines.h"

#include <memory>

namespace tudov
{
	class Engine;
	class DynamicBuffer;

	class Renderer
	{
	  public:
		struct DrawArgs
		{
			ResourceID tex;
			SDL_Rect src;
			SDL_Rect dst;
			std::float_t z;
			SDL_Color color;
			std::float_t angle;
			glm::vec2 center;
		};

		struct DrawTextArgs
		{
			std::string_view text;
			std::vector<FontID> fonts;
			std::int32_t fontSize;
			std::float_t x;
			std::float_t y;
			std::float_t z;
			std::float_t scale;
			SDL_Color color;
			glm::vec2 align;
			std::float_t wrapWidth;
			std::float_t shadow;
			SDL_Color shadowColor;
		};

	  private:
		std::string _viewName;

	  public:
		Window &window;

	  public:
		explicit Renderer(Window &window) noexcept;

		// CreateStaticBuffer();

		void Initialize() noexcept;
		void Begin() noexcept;
		void End() noexcept;
		glm::mat3x3 GetTransform() noexcept;
		void SetTransform(const glm::mat3x3 &mat3) noexcept;
		void Sort() noexcept;
		void Submit() noexcept;
		sol::object LuaGetTransform() noexcept;
		void LuaSetTransform(const sol::table &mat3) noexcept;
		std::shared_ptr<DynamicBuffer> LuaNewDynamicBuffer() noexcept;
	};
} // namespace tudov
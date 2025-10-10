/**
 * @file graphic/GUI.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Util/Definitions.hpp"
#include "Util/Micros.hpp"

#include <cmath>
#include <unordered_set>

struct ImFont;
struct SDL_Renderer;

namespace tudov
{
	class GUI
	{
	  private:
		inline static bool _isOn = false;
		inline static std::float_t _scale = 1;
		inline static std::unordered_set<SDL_Renderer *> _sdlRenderers{};

	  public:
		TE_STATIC_CLASS(GUI);

		static void TryInit() noexcept;
		static void Quit() noexcept;

		static bool Attach(SDL_Renderer *sdlRenderer) noexcept;
		static bool Detach(SDL_Renderer *sdlRenderer) noexcept;

		static std::float_t GetScale() noexcept;
		static void SetScale(std::float_t scale) noexcept;

		static ImFont &GetSmallFont() noexcept;
		static ImFont &GetMediumFont() noexcept;
		static ImFont &GetLargeFont() noexcept;
	};
} // namespace tudov

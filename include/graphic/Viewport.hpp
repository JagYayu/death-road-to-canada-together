#pragma once

struct SDL_GPUViewport;

namespace tudov
{
	class Viewport
	{
	  private:
		SDL_GPUViewport *_viewport;

	  public:
		explicit Viewport() noexcept;
	};
} // namespace tudov

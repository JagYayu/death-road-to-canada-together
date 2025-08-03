#pragma once

struct SDL_Device;

namespace tudov
{
	class Device
	{
	  public:
		[[deprecated]]
		static void Initialize();
		[[deprecated]]
		static void Deinitialize();
		[[deprecated]]
		static SDL_Device *Get();

	  public:
		SDL_Device *GetRaw() noexcept;
	};
} // namespace tudov

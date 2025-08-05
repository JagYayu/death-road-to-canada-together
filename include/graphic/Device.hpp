#pragma once

struct SDL_Device;

namespace tudov
{
	class Device
	{
	  public:
		static void Initialize();
		static void Deinitialize();
		static SDL_Device *Get();
	};
} // namespace tudov

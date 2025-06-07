#pragma once

#include "ModConfig.hpp"

namespace tudov
{
	class ModManager;

	class Mod
	{
	protected:
		ModManager &_modManager;
		ModConfig _config;

	public:
		Mod(ModManager &modManager);
		explicit Mod(ModManager &modManager, const ModConfig &config);

		virtual void Load() = 0;
		virtual void Unload() = 0;

		void Reload();
	};
}

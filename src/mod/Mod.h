#pragma once

#include "ModConfig.hpp"

#include <string>

namespace tudov
{
	class ModManager;

	class Mod
	{
	  protected:
		ModConfig _config;

		Vector<String> _scripts;

	  public:
		ModManager &modManager;

		Mod(ModManager &modManager);
		explicit Mod(ModManager &modManager, const ModConfig &config);

		virtual void Load() = 0;
		virtual void Unload() = 0;

		void Reload();

		const String &GetNamespace() const;
		const String &GetScriptsDirectory() const;
	};
} // namespace tudov

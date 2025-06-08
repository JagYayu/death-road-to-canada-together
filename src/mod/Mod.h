#pragma once

#include "ModConfig.hpp"

#include <string>

namespace tudov
{
	class ModManager;

	class Mod
	{
	  protected:
		ModManager &_modManager;
		ModConfig _config;

		std::vector<std::string> _scripts;

	  public:
		Mod(ModManager &modManager);
		explicit Mod(ModManager &modManager, const ModConfig &config);

		virtual void Load() = 0;
		virtual void Unload() = 0;

		void Reload();

		const std::string &GetNamespace() const;
		const std::string &GetScriptsDirectory() const;
	};
} // namespace tudov

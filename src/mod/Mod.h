#pragma once

#include "ModConfig.hpp"
#include "ScriptProvider.h"
#include "util/Defs.h"

#include <string>

namespace tudov
{
	class ModManager;

	class Mod
	{
	  protected:
		ModConfig _config;

		std::vector<ScriptID> _scripts;

	  public:
		ModManager &modManager;

		Mod(ModManager &modManager);
		explicit Mod(ModManager &modManager, const ModConfig &config);

		virtual void Load() = 0;
		virtual void Unload() = 0;

		void Reload();

		ModConfig &GetConfig() noexcept;
		const ModConfig &GetConfig() const noexcept;
		const std::string &GetNamespace() const noexcept;
		const std::string &GetScriptsDirectory() const noexcept;
	};
} // namespace tudov

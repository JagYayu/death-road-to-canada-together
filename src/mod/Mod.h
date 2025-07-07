#pragma once

#include "ModConfig.hpp"
#include "ScriptProvider.h"
#include "util/Defs.h"

#include <string>

namespace tudov
{
	class ModManager;

	struct IMod
	{
		virtual void Load() = 0;
		virtual void Unload() = 0;

		inline void Reload()
		{
			Unload();
			Load();
		}

		virtual ModManager &GetModManager() noexcept = 0;
		virtual ModConfig &GetConfig() noexcept = 0;
		virtual const ModConfig &GetConfig() const noexcept = 0;
		virtual const std::string &GetNamespace() const noexcept = 0;
		virtual const std::string &GetScriptsDirectory() const noexcept = 0;
	};

	class Mod : public IMod
	{
	  protected:
		ModManager &_modManager;
		ModConfig _config;

		std::vector<ScriptID> _scripts;
		std::vector<FontID> _fonts;

	  public:
		explicit Mod(ModManager &modManager);
		explicit Mod(ModManager &modManager, const ModConfig &config);

		ModManager &GetModManager() noexcept;
		ModConfig &GetConfig() noexcept;
		const ModConfig &GetConfig() const noexcept;
		const std::string &GetNamespace() const noexcept;
		const std::string &GetScriptsDirectory() const noexcept;
	};
} // namespace tudov

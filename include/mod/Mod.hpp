/**
 * @file mod/Mod.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "ModConfig.hpp"
#include "util/Definitions.hpp"

#include <string>

namespace tudov
{
	class ModManager;

	struct IMod
	{
		virtual ~IMod() noexcept = default;

		virtual void Load() = 0;
		virtual void Unload() = 0;

		virtual ModManager &GetModManager() noexcept = 0;
		virtual ModConfig &GetConfig() noexcept = 0;

		virtual void Update()
		{
		}

		inline void Reload()
		{
			Unload();
			Load();
		}

		inline const ModConfig &GetConfig() const noexcept
		{
			return const_cast<IMod *>(this)->GetConfig();
		}

		[[deprecated("use `GetConfig().namespace_` or `_config.namespace_` instead")]]
		inline std::string_view GetNamespace() const noexcept
		{
			return GetConfig().namespace_;
		}

		[[deprecated("use `GetConfig().scripts.directory` instead")]]
		inline std::string_view GetScriptsDirectory() const noexcept
		{
			return GetConfig().scripts.directory;
		}
	};

	class Mod : public virtual IMod
	{
	  protected:
		ModManager &_modManager;
		ModConfig _config;

		std::vector<std::string> _scripts;
		std::vector<FontID> _fonts;

	  public:
		explicit Mod(ModManager &modManager);
		explicit Mod(ModManager &modManager, const ModConfig &config);

		ModManager &GetModManager() noexcept override;
		ModConfig &GetConfig() noexcept override;

		bool ShouldScriptLoad(std::string_view relativePath) noexcept;
	};
} // namespace tudov

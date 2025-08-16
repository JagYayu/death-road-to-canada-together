/**
 * @file mod/Mod.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "mod/Mod.hpp"

using namespace tudov;

Mod::Mod(ModManager &modManager)
    : _modManager(modManager)
{
}

Mod::Mod(ModManager &modManager, const ModConfig &config)
    : _modManager(modManager),
      _config(config)
{
}

ModManager &Mod::GetModManager() noexcept
{
	return _modManager;
}

ModConfig &Mod::GetConfig() noexcept
{
	return _config;
}

bool Mod::ShouldScriptLoad(std::string_view relativePath) noexcept
{
	// auto &scripts = GetConfig().scripts;
	// scripts.directoryClient;

	// TODO
	return true;
}

#include "Mod.h"

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

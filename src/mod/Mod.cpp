#include "Mod.h"

#include <filesystem>

using namespace tudov;

Mod::Mod(ModManager &modManager)
    : modManager(modManager)
{
}

Mod::Mod(ModManager &modManager, const ModConfig &config)
    : modManager(modManager),
      _config(config)
{
}

void Mod::Reload()
{
	Unload();
	Load();
}

ModConfig &Mod::GetConfig() noexcept
{
	return _config;
}

const ModConfig &Mod::GetConfig() const noexcept
{
	return _config;
}

const String &Mod::GetNamespace() const noexcept
{
	return _config.namespace_;
}

const String &Mod::GetScriptsDirectory() const noexcept
{
	return _config.scripts.directory;
}

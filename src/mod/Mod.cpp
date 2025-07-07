#include "Mod.h"

#include <filesystem>

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

const ModConfig &Mod::GetConfig() const noexcept
{
	return _config;
}

const std::string &Mod::GetNamespace() const noexcept
{
	return _config.namespace_;
}

const std::string &Mod::GetScriptsDirectory() const noexcept
{
	return _config.scripts.directory;
}

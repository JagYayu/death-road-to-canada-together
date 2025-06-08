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

void Mod::Reload()
{
	Unload();
	Load();
}

const std::string &Mod::GetNamespace() const
{
	return _config.namespace_;
}

const std::string &Mod::GetScriptsDirectory() const
{
	return _config.scripts.directory;
}

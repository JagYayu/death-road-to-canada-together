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

const String &Mod::GetNamespace() const
{
	return _config.namespace_;
}

const String &Mod::GetScriptsDirectory() const
{
	return _config.scripts.directory;
}

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

void Mod::Reload()
{
	Unload();
	Load();
}

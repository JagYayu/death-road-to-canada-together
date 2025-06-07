#pragma once

#include "Mod.h"

#include "FileWatch.hpp"

namespace tudov
{
	class UnpackagedMod : public Mod
	{
	private:
		std::filesystem::path _directory;
		std::shared_ptr<filewatch::FileWatch<std::wstring>> _watcher;

	public:
		static ModConfig LoadConfig(const std::filesystem::path &directory);

		explicit UnpackagedMod(ModManager& modManager, const std::filesystem::path &directory);

		void Load() override;
		void Unload() override;
	};
}

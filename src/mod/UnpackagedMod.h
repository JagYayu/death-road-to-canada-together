#pragma once

#include "Mod.h"
#include "util/Log.h"

#include "FileWatch.hpp"

namespace tudov
{
	class UnpackagedMod : public Mod
	{
	  private:
		SharedPtr<Log> _log;

		std::filesystem::path _directory;
		std::shared_ptr<filewatch::FileWatch<String>> _watcher;

	  public:
		static bool IsValidDirectory(const std::filesystem::path &directory);
		static ModConfig LoadConfig(const std::filesystem::path &directory);

		explicit UnpackagedMod(ModManager &modManager, const std::filesystem::path &directory);

		void Load() override;
		void Unload() override;
	};
} // namespace tudov

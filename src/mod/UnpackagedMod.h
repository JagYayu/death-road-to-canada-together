#pragma once

#include "Mod.h"
#include "util/Defs.h"
#include "util/Log.h"

#include <FileWatch.hpp>

namespace tudov
{
	class UnpackagedMod : public Mod
	{
	  private:
		SharedPtr<Log> _log;

		SharedPtr<filewatch::FileWatch<std::string>> _fileWatcher;
		std::filesystem::path _directory;

		std::vector<std::regex> _scriptFilePatterns;

	  public:
		SharedPtr<Log> log;

		static bool IsValidDirectory(const std::filesystem::path &directory);
		static ModConfig LoadConfig(const std::filesystem::path &directory);

		explicit UnpackagedMod(ModManager &modManager, const std::filesystem::path &directory);

	  private:
		void UpdateFilePatterns();

	  public:
		bool IsScript(const std::string &fileName) const;

		void Load() override;
		void Unload() override;
	};
} // namespace tudov

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

		SharedPtr<filewatch::FileWatch<String>> _fileWatcher;
		std::filesystem::path _directory;

		Vector<std::regex> _scriptFilePatterns;

	  public:
		SharedPtr<Log> log;

		static bool IsValidDirectory(const std::filesystem::path &directory);
		static ModConfig LoadConfig(const std::filesystem::path &directory);

		explicit UnpackagedMod(ModManager &modManager, const std::filesystem::path &directory);

	  private:
		void UpdateFilePatterns();

	  public:
		bool IsScript(const String &fileName) const;

		void Load() override;
		void Unload() override;
	};
} // namespace tudov

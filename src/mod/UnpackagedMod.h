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
		std::shared_ptr<Log> _log;

		std::shared_ptr<filewatch::FileWatch<std::string>> _fileWatcher;
		std::filesystem::path _directory;

		std::vector<std::regex> _scriptFilePatterns;
		std::vector<std::regex> _fontFilePatterns;

	  public:
		std::shared_ptr<Log> log;

		static bool IsValidDirectory(const std::filesystem::path &directory);
		static ModConfig LoadConfig(const std::filesystem::path &directory);

		explicit UnpackagedMod(ModManager &modManager, const std::filesystem::path &directory);

	  private:
		void UpdateFilePatterns();

	  public:
		bool IsScript(std::string_view file) const;
		bool IsFont(std::string_view file) const;

		void Load() override;
		void Unload() override;
	};
} // namespace tudov

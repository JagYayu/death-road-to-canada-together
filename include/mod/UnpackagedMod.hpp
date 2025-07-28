#pragma once

#include "Mod.hpp"
#include "program/Context.hpp"
#include "util/Definitions.hpp"
#include "util/Log.hpp"

#include "FileWatch.hpp"

#include <memory>
#include <regex>

namespace tudov
{
	struct IUnpackagedMod : virtual public IMod
	{
	};

	class UnpackagedMod : public Mod, public IUnpackagedMod, public IContextProvider
	{
	  private:
		std::shared_ptr<Log> _log;

		bool _loaded;
		std::unique_ptr<filewatch::FileWatch<std::string>> _fileWatcher;
		std::filesystem::path _directory;

		std::vector<std::regex> _scriptFilePatterns;
		std::vector<std::regex> _fontFilePatterns;

	  public:
		explicit UnpackagedMod(ModManager &modManager, const std::filesystem::path &directory);
		virtual ~UnpackagedMod() noexcept = default;

	  private:
		void UpdateFilePatterns();

	  public:
		Context &GetContext() noexcept override;

		bool IsScript(std::string_view file) const;
		bool IsFont(std::string_view file) const;

		bool IsValidDirectory(const std::filesystem::path &directory);
		ModConfig LoadConfig(const std::filesystem::path &directory);

		void Load() override;
		void Unload() override;
	};
} // namespace tudov

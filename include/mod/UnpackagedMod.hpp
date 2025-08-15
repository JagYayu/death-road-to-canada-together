#pragma once

#include "Mod.hpp"
#include "program/Context.hpp"
#include "util/Log.hpp"

#include <memory>
#include <queue>
#include <regex>
#include <tuple>

namespace tudov
{
	struct IUnpackagedMod : public virtual IMod
	{
		virtual std::filesystem::path GetDirectory() noexcept = 0;
	};

	class UnpackagedMod : public Mod, public IUnpackagedMod, public IContextProvider, private ILogProvider
	{
	  private:
		std::shared_ptr<Log> _log;

		bool _loaded;
		void *_fileWatcher;
		std::filesystem::path _directory;

		std::vector<std::regex> _scriptFilePatterns;
		std::vector<std::regex> _fontFilePatterns;

		std::queue<std::tuple<std::filesystem::path, int>> _fileWatchQueue;
		std::mutex _fileWatchMutex;

	  public:
		explicit UnpackagedMod(ModManager &modManager, const std::filesystem::path &directory);
		~UnpackagedMod() noexcept override;

	  public:
		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		std::filesystem::path GetDirectory() noexcept override;
		
		void Update() override;

		bool IsScript(std::string_view file) const;
		bool IsFont(std::string_view file) const;

		bool IsValidDirectory(const std::filesystem::path &directory);
		ModConfig LoadConfig(const std::filesystem::path &directory);

		void Load() override;
		void Unload() override;

	  private:
		void UpdateFileMatchPatterns();
		void ScriptAdded(const std::filesystem::path &file) noexcept;
		void ScriptRemoved(const std::filesystem::path &file) noexcept;
		void ScriptModified(const std::filesystem::path &file) noexcept;
	};
} // namespace tudov

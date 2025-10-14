/**
 * @file Mod/UnpackagedMod.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Mod.hpp"
#include "Program/Context.hpp"
#include "System/Log.hpp"

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

	enum class EFileChangeType : int;
	enum class EPathType : int;
	class FileSystemWatch;

	class UnpackagedMod final : public Mod, public IUnpackagedMod, public IContextProvider, private ILogProvider
	{
	  private:
		std::shared_ptr<Log> _log;

		bool _loaded;
		std::unique_ptr<FileSystemWatch> _fileWatcher;
		std::filesystem::path _directory;

		std::vector<std::regex> _scriptFilePatterns;
		std::vector<std::regex> _fontFilePatterns;

		std::queue<std::tuple<std::filesystem::path, EPathType, EFileChangeType>> _fileWatchQueue;
		std::mutex _fileWatchMutex;

	  public:
		explicit UnpackagedMod(ModManager &modManager, const std::filesystem::path &directory);
		explicit UnpackagedMod(const UnpackagedMod &) noexcept = delete;
		explicit UnpackagedMod(UnpackagedMod &&) noexcept = delete;
		UnpackagedMod &operator=(const UnpackagedMod &) noexcept = delete;
		UnpackagedMod &operator=(UnpackagedMod &&) noexcept = delete;
		~UnpackagedMod() noexcept override;

	  public:
		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		std::filesystem::path GetDirectory() noexcept override;

		void Update() override;

		/**
		 * Check file's extension to determine if it's a script file.
		 */
		bool IsScript(std::string_view file) const;
		/**
		 * Check file's extension to determine if it's a font file.
		 */
		bool IsFont(std::string_view file) const;

		bool IsValidDirectory(const std::filesystem::path &directory);
		ModConfig LoadConfig(const std::filesystem::path &directory);

		void Load() override;
		void Unload() override;

	  private:
		void UpdateFileMatchPatterns();
		void ScriptAdded(const std::filesystem::path &file) noexcept;
		bool ScriptRemoved(const std::filesystem::path &file) noexcept;
		bool ScriptModified(const std::filesystem::path &file) noexcept;
		void FileAdded(const std::filesystem::path &file) noexcept;
		void FileRemoved(const std::filesystem::path &file) noexcept;
		void FileModified(const std::filesystem::path &file) noexcept;
	};
} // namespace tudov

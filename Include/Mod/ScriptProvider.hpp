/**
 * @file Mod/ScriptProvider.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Program/EngineComponent.hpp"
#include "System/Log.hpp"
#include "Util/Definitions.hpp"
#include "Util/StringUtils.hpp"

#include <optional>
#include <string>
#include <unordered_map>

namespace tudov
{
	class Mod;
	class Text;

	struct IScriptProvider : public IEngineComponent
	{
	  public:
		struct IteratorEntry
		{
			ScriptID scriptID;
			std::string name;
			TextID textID;
			std::string_view modUID;
		};

	  protected:
		struct Entry
		{
			std::string name;
			TextID textID;
			std::string_view modUID;
		};

	  public:
		using TScriptID2Entry = std::unordered_map<ScriptID, Entry>;

	  public:
		virtual ~IScriptProvider() noexcept = default;

		virtual std::uint64_t GetVersionID() const noexcept = 0;
		virtual size_t GetCount() const noexcept = 0;
		virtual ScriptID GetScriptIDByName(std::string_view scriptName) const noexcept = 0;
		virtual std::optional<std::string_view> GetScriptNameByID(ScriptID scriptID) const noexcept = 0;
		virtual bool IsValidScript(ScriptID scriptID) const noexcept = 0;
		virtual bool IsStaticScript(std::string_view scriptName) const noexcept = 0;

		virtual ScriptID AddScript(std::string_view scriptName, TextID scriptTextID, std::string_view modUID) noexcept = 0;
		virtual bool RemoveScript(ScriptID scriptID) noexcept = 0;
		virtual std::size_t RemoveScriptsBy(std::string_view modUID) noexcept = 0;
		virtual TextID GetScriptTextID(ScriptID scriptID) const noexcept = 0;
		virtual std::shared_ptr<Text> GetScriptCode(ScriptID scriptID) const noexcept = 0;
		/*
		 * Return an empty string if script id not exists.
		 */
		virtual std::string_view GetScriptModUID(ScriptID scriptID) noexcept = 0;

		virtual std::size_t GetEntriesSize() const = 0;
		virtual std::vector<IteratorEntry>::const_iterator BeginEntries() const = 0;
		virtual std::vector<IteratorEntry>::const_iterator EndEntries() const = 0;

		inline bool IsStaticScript(ScriptID scriptID) const noexcept
		{
			auto &&scriptName = GetScriptNameByID(scriptID);
			if (!scriptName.has_value())
			{
				return false;
			}
			return IsStaticScript(scriptName->data());
		}

		inline bool RemoveScript(std::string_view scriptName) noexcept
		{
			return RemoveScript(GetScriptIDByName(scriptName));
		}

		inline TextID GetScriptTextID(std::string_view scriptName) const noexcept
		{
			return GetScriptTextID(GetScriptIDByName(scriptName));
		}

		inline std::shared_ptr<Text> GetScriptCode(std::string_view scriptName) const noexcept
		{
			return GetScriptCode(GetScriptIDByName(scriptName));
		}

		inline std::string_view GetScriptModUID(std::string_view scriptName) noexcept
		{
			return GetScriptModUID(GetScriptIDByName(scriptName));
		}

		inline decltype(auto) begin() const
		{
			return BeginEntries();
		}
		inline decltype(auto) end() const
		{
			return EndEntries();
		}
	};

	class Log;

	class ScriptProvider final : public IScriptProvider, private ILogProvider
	{
	  public:
		using ScriptID = std::uint64_t;

	  private:
		Context &_context;
		std::shared_ptr<Log> _log;

		std::uint64_t _versionID;
		ScriptID _latestScriptID;
		std::unordered_map<std::string_view, ScriptID> _scriptName2ID;
		TScriptID2Entry _scriptID2Entry;

		mutable std::optional<std::vector<IteratorEntry>> _iteratorEntries;

	  public:
		explicit ScriptProvider(Context &context) noexcept;
		explicit ScriptProvider(const ScriptProvider &) noexcept = delete;
		explicit ScriptProvider(ScriptProvider &&) noexcept = delete;
		ScriptProvider &operator=(const ScriptProvider &) noexcept = delete;
		ScriptProvider &operator=(ScriptProvider &&) noexcept = delete;
		~ScriptProvider() override;

	  private:
		ScriptID AddScriptImpl(std::string_view scriptName, TextID scriptTextID, std::string_view modUID = emptyString);
		bool RemoveScriptImpl(ScriptID scriptID) noexcept;

	  public:
		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		void Initialize() noexcept override;
		void Deinitialize() noexcept override;

		std::uint64_t GetVersionID() const noexcept override;
		size_t GetCount() const noexcept override;
		ScriptID GetScriptIDByName(std::string_view scriptName) const noexcept override;
		std::optional<std::string_view> GetScriptNameByID(ScriptID scriptID) const noexcept override;
		bool IsValidScript(ScriptID scriptID) const noexcept override;
		bool IsStaticScript(std::string_view scriptName) const noexcept override;

		ScriptID AddScript(std::string_view scriptName, TextID scriptTextID, std::string_view modUID) noexcept override;
		bool RemoveScript(ScriptID scriptID) noexcept override;
		std::size_t RemoveScriptsBy(std::string_view modUID) noexcept override;
		TextID GetScriptTextID(ScriptID scriptID) const noexcept override;
		std::shared_ptr<Text> GetScriptCode(ScriptID scriptID) const noexcept override;
		std::string_view GetScriptModUID(ScriptID scriptID) noexcept override;

		std::size_t GetEntriesSize() const override;
		std::vector<IteratorEntry>::const_iterator BeginEntries() const override;
		std::vector<IteratorEntry>::const_iterator EndEntries() const override;

	  protected:
		void UpdateScriptEntriesCache() const noexcept;
	};
} // namespace tudov

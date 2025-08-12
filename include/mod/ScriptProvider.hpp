#pragma once

#include "program/EngineComponent.hpp"
#include "util/Definitions.hpp"
#include "util/Log.hpp"
#include "util/StringUtils.hpp"

#include <optional>
#include <string>
#include <unordered_map>

namespace tudov
{
	class Mod;
	class TextResource;

	struct IScriptProvider : public IEngineComponent
	{
	  protected:
		struct Entry
		{
			std::string name;
			std::shared_ptr<TextResource> code;
			std::string_view modUID;
		};

	  public:
		using TScriptID2Entry = std::unordered_map<ScriptID, Entry>;

	  public:
		virtual ~IScriptProvider() noexcept = default;

		virtual size_t GetCount() const noexcept = 0;
		virtual ScriptID GetScriptIDByName(std::string_view scriptName) const noexcept = 0;
		virtual std::optional<std::string_view> GetScriptNameByID(ScriptID scriptID) const noexcept = 0;
		virtual bool IsValidScript(ScriptID scriptID) const noexcept = 0;
		virtual bool IsStaticScript(std::string_view scriptName) const noexcept = 0;

		virtual ScriptID AddScript(std::string_view scriptName, const std::shared_ptr<TextResource> &scriptCode, std::string_view modUID) noexcept = 0;
		virtual bool RemoveScript(ScriptID scriptID) noexcept = 0;
		virtual std::size_t RemoveScriptsBy(std::string_view modUID) noexcept = 0;
		virtual std::shared_ptr<TextResource> GetScriptCode(ScriptID scriptID) const noexcept = 0;
		/*
		 * Return an empty string if script id not exists.
		 */
		virtual std::string_view GetScriptModUID(ScriptID scriptID) noexcept = 0;

		virtual TScriptID2Entry::const_iterator begin() const = 0;
		virtual TScriptID2Entry::const_iterator end() const = 0;

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

		inline std::shared_ptr<TextResource> GetScriptCode(std::string_view scriptName) const noexcept
		{
			return GetScriptCode(GetScriptIDByName(scriptName));
		}

		inline std::string_view GetScriptModUID(std::string_view scriptName) noexcept
		{
			return GetScriptModUID(GetScriptIDByName(scriptName));
		}
	};

	class Log;

	class ScriptProvider : public IScriptProvider, public ILogProvider
	{
	  public:
		using ScriptID = std::uint64_t;

	  private:
		Context &_context;
		std::shared_ptr<Log> _log;

		ScriptID _latestScriptID;
		std::unordered_map<std::string_view, ScriptID> _scriptName2ID;
		TScriptID2Entry _scriptID2Entry;

	  public:
		explicit ScriptProvider(Context &context) noexcept;
		~ScriptProvider() override;

	  private:
		ScriptID AddScriptImpl(std::string_view scriptName, const std::shared_ptr<TextResource> &scriptCode, std::string_view modUID = emptyString);
		bool RemoveScriptImpl(ScriptID scriptID) noexcept;

	  public:
		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		void Initialize() noexcept override;
		void Deinitialize() noexcept override;

		size_t GetCount() const noexcept override;
		ScriptID GetScriptIDByName(std::string_view scriptName) const noexcept override;
		std::optional<std::string_view> GetScriptNameByID(ScriptID scriptID) const noexcept override;
		bool IsValidScript(ScriptID scriptID) const noexcept override;
		bool IsStaticScript(std::string_view scriptName) const noexcept override;

		ScriptID AddScript(std::string_view scriptName, const std::shared_ptr<TextResource> &scriptCode, std::string_view modUID) noexcept override;
		bool RemoveScript(ScriptID scriptID) noexcept override;
		std::size_t RemoveScriptsBy(std::string_view modUID) noexcept override;
		std::shared_ptr<TextResource> GetScriptCode(ScriptID scriptID) const noexcept override;
		std::string_view GetScriptModUID(ScriptID scriptID) noexcept override;

		std::unordered_map<ScriptID, Entry>::const_iterator begin() const override;
		std::unordered_map<ScriptID, Entry>::const_iterator end() const override;
	};
} // namespace tudov

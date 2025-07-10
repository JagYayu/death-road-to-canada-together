#pragma once

#include "mod/Mod.hpp"
#include "program/EngineComponent.hpp"
#include "util/Defs.hpp"
#include "util/Log.hpp"
#include "util/StringUtils.hpp"

#include <functional>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace tudov
{
	class Mod;

	struct IScriptProvider : public IEngineComponent
	{
	  protected:
		struct Entry
		{
			std::string name;
			std::string code;
			std::string_view namespace_;
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

		virtual ScriptID AddScript(std::string_view scriptName, std::string_view scriptCode, std::string_view namespace_) noexcept = 0;
		virtual bool RemoveScript(ScriptID scriptID) noexcept = 0;
		virtual std::size_t RemoveScriptBy(std::string_view namespace_) noexcept = 0;
		virtual const std::string &GetScriptCode(ScriptID scriptID) const noexcept = 0;
		/*
		 * Return an empty string if script id not exists.
		 */
		virtual std::string_view GetScriptNamespace(ScriptID scriptID) noexcept = 0;

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

		inline const std::string &GetScriptCode(std::string_view scriptName) const noexcept
		{
			return GetScriptCode(GetScriptIDByName(scriptName));
		}

		inline std::string_view GetScriptNamespace(std::string_view scriptName) noexcept
		{
			return GetScriptNamespace(GetScriptIDByName(scriptName));
		}
	};

	class ScriptProvider : public IScriptProvider
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
		ScriptID AddScriptImpl(std::string_view scriptName, std::string_view scriptCode, std::string_view namespace_ = emptyString);
		bool RemoveScriptImpl(ScriptID scriptID) noexcept;

	  public:
		Context &GetContext() noexcept override;

		size_t GetCount() const noexcept override;
		ScriptID GetScriptIDByName(std::string_view scriptName) const noexcept override;
		std::optional<std::string_view> GetScriptNameByID(ScriptID scriptID) const noexcept override;
		bool IsValidScript(ScriptID scriptID) const noexcept override;
		bool IsStaticScript(std::string_view scriptName) const noexcept override;

		ScriptID AddScript(std::string_view scriptName, std::string_view scriptCode, std::string_view namespace_) noexcept override;
		bool RemoveScript(ScriptID scriptID) noexcept override;
		std::size_t RemoveScriptBy(std::string_view namespace_) noexcept override;
		const std::string &GetScriptCode(ScriptID scriptID) const noexcept override;
		std::string_view GetScriptNamespace(ScriptID scriptID) noexcept override;

		std::unordered_map<ScriptID, Entry>::const_iterator begin() const override;
		std::unordered_map<ScriptID, Entry>::const_iterator end() const override;
	};
} // namespace tudov

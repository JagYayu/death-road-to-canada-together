#pragma once

#include "mod/Mod.h"
#include "program/IEngineComponent.h"
#include "util/Defs.h"
#include "util/Log.h"
#include "util/StringUtils.hpp"

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
		virtual size_t GetCount() const noexcept = 0;
		virtual ScriptID GetScriptIDByName(std::string_view scriptName) const noexcept = 0;
		virtual std::optional<std::string_view> GetScriptNameByID(ScriptID scriptID) const noexcept = 0;
		virtual bool IsValidScriptID(ScriptID scriptID) const noexcept = 0;
		virtual bool IsStaticScript(ScriptID scriptID) const noexcept = 0;

		virtual ScriptID AddScript(std::string_view scriptName, std::string_view scriptCode, std::string_view namespace_) noexcept = 0;
		virtual void RemoveScript(ScriptID scriptID) noexcept = 0;
		virtual const std::string &GetScriptCode(ScriptID scriptID) const noexcept = 0;
		virtual std::string_view GetScriptNamespace(ScriptID scriptID) noexcept = 0;

		virtual std::unordered_map<ScriptID, Entry>::const_iterator begin() const = 0;
		virtual std::unordered_map<ScriptID, Entry>::const_iterator end() const = 0;
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
		std::unordered_map<ScriptID, Entry> _scriptID2Entry;

	  public:
		static bool IsStaticScript(std::string_view scriptName);

	  public:
		ScriptProvider(Context &context);

	  private:
		ScriptID AddScriptImpl(std::string_view scriptName, std::string_view scriptCode, std::string_view namespace_ = emptyString);
		void RemoveScriptImpl(ScriptID scriptID);

	  public:
		Context &GetContext() noexcept override;

		size_t GetCount() const noexcept override;
		ScriptID GetScriptIDByName(std::string_view scriptName) const noexcept override;
		std::optional<std::string_view> GetScriptNameByID(ScriptID scriptID) const noexcept override;
		bool IsValidScriptID(ScriptID scriptID) const noexcept override;
		bool IsStaticScript(ScriptID scriptID) const noexcept override;

		ScriptID AddScript(std::string_view scriptName, std::string_view scriptCode, std::string_view namespace_) noexcept override;
		void RemoveScript(ScriptID scriptID) noexcept override;
		const std::string &GetScriptCode(ScriptID scriptID) const noexcept override;
		std::string_view GetScriptNamespace(ScriptID scriptID) noexcept override;

		std::unordered_map<ScriptID, Entry>::const_iterator begin() const override;
		std::unordered_map<ScriptID, Entry>::const_iterator end() const override;
	};
} // namespace tudov

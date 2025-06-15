#pragma once

#include "mod/Mod.h"
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
	class ModManager;

	class ScriptProvider
	{
	  private:
		struct Entry
		{
			std::string name;
			std::string code;
			std::string_view namespace_;
		};

	  public:
		using ScriptID = UInt64;

	  private:
		SharedPtr<Log> _log;

		ScriptID _latestScriptID;
		std::unordered_map<std::string_view, ScriptID> _scriptName2ID;
		std::unordered_map<ScriptID, Entry> _scriptID2Entry;

	  public:
		ModManager &modManager;

	  public:
		static bool IsStaticScript(std::string_view scriptName);

	  public:
		ScriptProvider(ModManager &modManager);

	  private:
		ScriptID AllocScript(std::string_view scriptName, std::string_view scriptCode, std::string_view namespace_ = emptyString);
		void DeallocScript(ScriptID scriptID);

	  public:
		void Initialize();

		size_t GetCount() const noexcept;
		ScriptID GetScriptIDByName(std::string_view scriptName) const noexcept;
		std::optional<std::string_view> GetScriptNameByID(ScriptID scriptID) const noexcept;
		bool IsValidScriptID(ScriptID scriptID) const noexcept;
		bool IsStaticScript(ScriptID scriptID) const noexcept;

		ScriptID AddScript(std::string_view scriptName, std::string_view scriptCode, std::string_view namespace_) noexcept;
		void RemoveScript(ScriptID scriptName) noexcept;
		const std::string &GetScriptCode(ScriptID scriptID) const noexcept;
		std::string_view GetScriptNamespace(ScriptID scriptID) noexcept;

		std::unordered_map<ScriptID, Entry>::const_iterator begin() const;
		std::unordered_map<ScriptID, Entry>::const_iterator end() const;
	};
} // namespace tudov

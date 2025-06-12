#pragma once

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
	class ModManager;

	class ScriptProvider
	{
	  private:
		struct Entry
		{
			String name;
			String code;
		};

	  public:
		using ScriptID = UInt64;

	  private:
		SharedPtr<Log> _log;

		ScriptID _latestScriptID;
		UnorderedMap<StringView, ScriptID> _scriptName2ID;
		UnorderedMap<ScriptID, Entry> _scriptID2Entry;

	  public:
		ModManager &modManager;

	  public:
		static bool IsStaticScript(StringView scriptName);

	  public:
		ScriptProvider(ModManager &modManager);

	  private:
		ScriptID AllocScript(StringView scriptName, StringView scriptCode);
		void DeallocScript(ScriptID scriptID);

	  public:
		void Initialize();

		size_t GetCount() const noexcept;
		ScriptID GetScriptIDByName(StringView scriptName) const noexcept;
		Optional<StringView> GetScriptNameByID(ScriptID scriptID) const noexcept;
		bool IsValidScriptID(ScriptID scriptID) const noexcept;
		bool IsStaticScript(ScriptID scriptID) const noexcept;

		ScriptID AddScript(StringView scriptName, StringView scriptCode) noexcept;
		void RemoveScript(ScriptID scriptName) noexcept;
		const String &GetScriptCode(ScriptID scriptID) const noexcept;

		UnorderedMap<ScriptID, Entry>::const_iterator begin() const;
		UnorderedMap<ScriptID, Entry>::const_iterator end() const;
	};
} // namespace tudov

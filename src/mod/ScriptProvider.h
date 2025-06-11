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
			StringView name;
			String code;
		};

	  public:
		using ScriptID = UInt64;

		static constexpr ScriptID invalidScriptID = 0;

	  private:
		SharedPtr<Log> _log;

		ScriptID _currentScriptID;
		UnorderedMap<String, ScriptID> _scriptIDs;
		UnorderedMap<ScriptID, Entry> _scriptMap;

	  public:
		ModManager &modManager;

	  public:
		static bool IsStaticScript(StringView scriptName);

	  public:
		ScriptProvider(ModManager &modManager);

	  private:
		Tuple<ScriptID, StringView> AllocScript(StringView scriptName);

	  public:
		void Initialize();

		size_t GetCount() const noexcept;
		ScriptID GetScriptID(StringView scriptName) const noexcept;
		bool IsValidScriptID(ScriptID scriptID) const noexcept;
		Optional<StringView> GetScriptName(ScriptID scriptID) const noexcept;

		ScriptID AddScript(StringView scriptName, StringView scriptCode) noexcept;
		void RemoveScript(ScriptID scriptName) noexcept;
		const String &GetScriptCode(ScriptID scriptID) const noexcept;

		UnorderedMap<ScriptID, Entry>::const_iterator begin() const;
		UnorderedMap<ScriptID, Entry>::const_iterator end() const;
	};
} // namespace tudov

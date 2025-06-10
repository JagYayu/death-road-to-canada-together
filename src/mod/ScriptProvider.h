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
		SharedPtr<Log> _log;

		UnorderedMap<String, String, StringSVHash, StringSVEqual> _scriptMap;

	  public:
		ModManager &modManager;
		String staticNamespace;

	  public:
		ScriptProvider(ModManager &modManager);

		void Initialize();

		size_t GetCount() const;

		bool ContainsScript(const StringView &scriptName) const;
		const String &GetScript(const String &scriptName) const;
		Optional<String> TryRequireScript(const StringView &scriptName);
		void AddScript(const StringView &name, const StringView &data);
		void RemoveScript(const StringView &name);

		UnorderedMap<String, String, StringSVHash, StringSVEqual>::const_iterator begin() const;
		UnorderedMap<String, String, StringSVHash, StringSVEqual>::const_iterator end() const;
	};
} // namespace tudov

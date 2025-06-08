#pragma once

#include "util/Log.h"

#include <map>
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
		ModManager &_modManager;

		Log _log;

		std::map<std::string, std::string> _scriptMap;

	  public:
		ScriptProvider(ModManager &modManager);

		size_t GetCount() const;

		void AddScript(const std::string &name, const std::string &data);
		void RemoveScript(const std::string &name);

		std::map<std::string, std::string>::const_iterator begin() const;
		std::map<std::string, std::string>::const_iterator end() const;
	};
} // namespace tudov

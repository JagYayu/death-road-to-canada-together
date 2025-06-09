#pragma once

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
		ModManager &_modManager;

		Log _log;

		std::unordered_map<std::string, std::string, StringTransHash, StringTransEqual> _scriptMap;

	  public:
		ScriptProvider(ModManager &modManager);

		void Initialize();

		size_t GetCount() const;

		bool ContainsScript(const std::string_view &name) const;
		const std::string &GetScript(const std::string &name) const;
		void AddScript(const std::string_view &name, const std::string_view &data);
		void RemoveScript(const std::string_view &name);

		std::unordered_map<std::string, std::string, StringTransHash, StringTransEqual>::const_iterator begin() const;
		std::unordered_map<std::string, std::string, StringTransHash, StringTransEqual>::const_iterator end() const;
	};
} // namespace tudov

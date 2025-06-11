#pragma once

#include "util/StringUtils.hpp"
#include "util/Version.h"

#include "lib/json.hpp"

namespace tudov
{
	struct ModConfig
	{
		struct Data
		{
			bool autoload{};
			String directory{};
			Vector<String> files{};
		};

		struct Scripts : public Data
		{
			bool closedSource{};
			Vector<String> permissions{};
			Vector<String> privates{};
		};

		String namespace_{};
		Version version{};
		String name{};
		String author{};
		String description{};
		UnorderedMap<String, Version> dependencies{};

		Data audios{};
		Scripts scripts{};
	};

	inline void from_json(const nlohmann::json &j, ModConfig::Data &d)
	{
		d.autoload = j.value("autoload", false);
		d.directory = j.value("directory", String());
		d.files = j.value("files", Vector<String>());
	}

	inline void from_json(const nlohmann::json &j, ModConfig::Scripts &s)
	{
		from_json(j, static_cast<ModConfig::Data &>(s));
		s.closedSource = j.value("closedSource", false);
		s.permissions = j.value("permissions", Vector<String>());
		s.privates = j.value("privates", Vector<String>());
	}

	inline void from_json(const nlohmann::json &j, ModConfig &m)
	{
		m.namespace_ = j.value("namespace", String());
		m.version = j.value("version", Version());
		m.name = j.value("name", String());
		m.author = j.value("author", String());
		m.description = j.value("description", String());
		m.dependencies = j.value("dependencies", UnorderedMap<String, Version>());
		m.audios = j.value("audios", ModConfig::Data());
		m.scripts = j.value("scripts", ModConfig::Scripts());
	}
} // namespace tudov
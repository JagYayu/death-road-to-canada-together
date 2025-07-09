#pragma once

#include "util/Version.hpp"

#include <json.hpp>

namespace tudov
{
	struct ModConfig
	{
		struct Data
		{
			bool autoload{};
			std::string directory{};
			std::vector<std::string> files{};
		};

		struct Scripts : public Data
		{
			bool closedSource{};
			bool sandbox{};
			std::vector<std::string> permissions{};
			std::vector<std::string> privates{};
		};

		std::string namespace_;
		Version version;
		std::string name;
		std::string author;
		std::string description;
		std::unordered_map<std::string, Version> dependencies;

		Data audios;
		Scripts scripts;
		Data fonts;
	};

	inline void from_json(const nlohmann::json &j, ModConfig::Data &d)
	{
		d.autoload = j.value("autoload", false);
		d.directory = j.value("directory", std::string());
		d.files = j.value("files", std::vector<std::string>());
	}

	inline void from_json(const nlohmann::json &j, ModConfig::Scripts &s)
	{
		from_json(j, static_cast<ModConfig::Data &>(s));
		s.closedSource = j.value("closedSource", false);
		s.sandbox = j.value("sandbox", false);
		s.permissions = j.value("permissions", std::vector<std::string>());
		s.privates = j.value("privates", std::vector<std::string>());
	}

	inline void from_json(const nlohmann::json &j, ModConfig &m)
	{
		m.namespace_ = j.value("namespace", std::string());
		m.version = j.value("version", Version());
		m.name = j.value("name", std::string());
		m.author = j.value("author", std::string());
		m.description = j.value("description", std::string());
		m.dependencies = j.value("dependencies", std::unordered_map<std::string, Version>());
		m.audios = j.value("audios", ModConfig::Data());
		m.scripts = j.value("scripts", ModConfig::Scripts());
		m.fonts = j.value("fonts", ModConfig::Data());
	}
} // namespace tudov
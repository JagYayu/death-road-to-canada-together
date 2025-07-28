#pragma once

#include "ModDistribution.hpp"
#include "util/Version.hpp"

#include <json.hpp>

namespace tudov
{
	struct ModConfig
	{
		struct Data
		{
			bool autoload = true;
			std::string directory = {};
			std::vector<std::string> files = {};
		};

		struct Scripts : public Data
		{
			bool closedSource = true;
			std::string directoryClient = "client";
			std::string directoryServer = "server";
			std::vector<std::string> permissions = {};
			[[deprecated("useless option, modders can define privates themselves")]]
			std::vector<std::string> privates = {};
			bool sandbox = true;
		};

		std::string uniqueID = "";
		std::string namespace_ = "";
		Version version = Version(1, 0, 0);
		std::string name = "";
		std::string author = "";
		std::string description = "";
		std::unordered_map<std::string, Version> dependencies = {};
		EModDistribution distribution;

		Data audios = Data{.directory = "audios"};
		Scripts scripts = Scripts{Data{.directory = "scripts"}};
		Data fonts = Data{.directory = "fonts"};
	};

	inline void from_json(const nlohmann::json &j, ModConfig::Data &d) noexcept
	{
		d.autoload = j.value("autoload", false);
		d.directory = j.value("directory", std::string());
		d.files = j.value("files", std::vector<std::string>());
	}

	inline void from_json(const nlohmann::json &j, ModConfig::Scripts &s) noexcept
	{
		from_json(j, static_cast<ModConfig::Data &>(s));
		s.closedSource = j.value("closedSource", false);
		s.directoryClient = j.value("directoryClient", "");
		s.directoryServer = j.value("directoryClient", "");
		s.permissions = j.value("permissions", std::vector<std::string>());
		s.privates = j.value("privates", std::vector<std::string>());
		s.sandbox = j.value("sandbox", false);
	}

	inline void from_json(const nlohmann::json &j, ModConfig &m) noexcept
	{
		m.uniqueID = j.value("uid", std::string());
		m.namespace_ = j.value("namespace", std::string());
		m.version = j.value("version", Version());
		m.name = j.value("name", std::string());
		m.author = j.value("author", std::string());
		m.description = j.value("description", std::string());
		m.dependencies = j.value("dependencies", std::unordered_map<std::string, Version>());
		m.distribution = j.value("distribution", EModDistribution::Universal);
		m.audios = j.value("audios", ModConfig::Data());
		m.scripts = j.value("scripts", ModConfig::Scripts());
		m.fonts = j.value("fonts", ModConfig::Data());
	}
} // namespace tudov
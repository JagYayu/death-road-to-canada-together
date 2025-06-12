#pragma once

#include "lib/FileWatch.hpp"
#include "lib/json.hpp"
#include "resource/ResourceType.hpp"
#include "util/Defs.h"
#include "util/Log.h"

#include <memory>
#include <string>

namespace tudov
{	
	class EngineConfig
	{
	  private:
		SharedPtr<Log> _log;
		nlohmann::json _config;
		UniquePtr<filewatch::FileWatch<String>> _fileWatcher;

	  public:
		static constexpr StringView file = "config.json";

		static constexpr StringView defaultWindowTitle = "DR2C";
		static constexpr UInt32 defaultWindowWidth = 1280;
		static constexpr UInt32 defaultWindowHeight = 720;

		EngineConfig();
		~EngineConfig();

		void Save() noexcept;
		void Load() noexcept;

		Vector<String> GetMountDirectories();
		UnorderedMap<String, ResourceType> GetMountFiles();
		StringView GetWindowTitle();
		UInt32 GetWindowWidth();
		UInt32 GetWindowHeight();

		void SetWindowTitle(const String &);
		void SetWindowWidth(UInt32);
		void SetWindowHeight(UInt32);
	};
} // namespace tudov

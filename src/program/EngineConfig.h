#pragma once

#include "lib/FileWatch.hpp"
#include "lib/json.hpp"

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

		void Save();
		void Load();

		StringView GetWindowTitle();
		UInt32 GetWindowWidth();
		UInt32 GetWindowHeight();

		void SetWindowTitle(const String &);
		void SetWindowWidth(UInt32);
		void SetWindowHeight(UInt32);
	};
} // namespace tudov

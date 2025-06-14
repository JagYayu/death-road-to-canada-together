#pragma once

#include "resource/ResourceType.hpp"
#include "util/Defs.h"
#include "util/Log.h"

#include <FileWatch.hpp>
#include <json.hpp>
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

		EngineConfig() noexcept;
		~EngineConfig() noexcept;

		void Save() noexcept;
		void Load() noexcept;

		Vector<String> GetMountDirectories() noexcept;
		UnorderedMap<String, ResourceType> GetMountFiles() noexcept;
		void GetDebugProfiling() noexcept;
		StringView GetWindowTitle() noexcept;
		UInt32 GetWindowWidth() noexcept;
		UInt32 GetWindowHeight() noexcept;

		void SetWindowTitle(const String &) noexcept;
		void SetWindowWidth(UInt32) noexcept;
		void SetWindowHeight(UInt32) noexcept;
	};
} // namespace tudov

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
	enum class ERenderBackend;

	class EngineConfig
	{
	  private:
		SharedPtr<Log> _log;
		nlohmann::json _config;
		UniquePtr<filewatch::FileWatch<std::string>> _fileWatcher;

	  public:
		static constexpr std::string_view file = "config.json";

		static constexpr std::string_view defaultWindowTitle = "DR2C";
		static constexpr UInt32 defaultWindowWidth = 1280;
		static constexpr UInt32 defaultWindowHeight = 720;

		EngineConfig() noexcept;
		~EngineConfig() noexcept;

		void Save() noexcept;
		void Load() noexcept;

		std::vector<std::string> GetMountDirectories() noexcept;
		std::unordered_map<std::string, ResourceType> GetMountFiles() noexcept;
		void GetDebugProfiling() noexcept;
		ERenderBackend GetRenderBackend() noexcept;
		UInt32 GetWindowFramelimit() noexcept;
		std::string_view GetWindowTitle() noexcept;
		UInt32 GetWindowWidth() noexcept;
		UInt32 GetWindowHeight() noexcept;

		void SetWindowTitle(const std::string &) noexcept;
		void SetWindowWidth(UInt32) noexcept;
		void SetWindowHeight(UInt32) noexcept;
	};
} // namespace tudov

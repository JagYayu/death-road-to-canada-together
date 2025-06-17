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
		std::shared_ptr<Log> _log;
		nlohmann::json _config;
		std::unique_ptr<filewatch::FileWatch<std::string>> _fileWatcher;

	  public:
		static constexpr std::string_view file = "config.json";

		static constexpr std::string_view defaultWindowTitle = "DR2C";
		static constexpr std::uint32_t defaultWindowWidth = 1280;
		static constexpr std::uint32_t defaultWindowHeight = 720;

		EngineConfig() noexcept;
		~EngineConfig() noexcept;

		void Save() noexcept;
		void Load() noexcept;

		std::vector<std::string> GetMountDirectories() noexcept;
		std::unordered_map<std::string, ResourceType> GetMountFiles() noexcept;
		void GetDebugProfiling() noexcept;
		ERenderBackend GetRenderBackend() noexcept;
		std::uint32_t GetWindowFramelimit() noexcept;
		std::string_view GetWindowTitle() noexcept;
		std::uint32_t GetWindowWidth() noexcept;
		std::uint32_t GetWindowHeight() noexcept;

		void SetWindowTitle(const std::string &) noexcept;
		void SetWindowWidth(std::uint32_t) noexcept;
		void SetWindowHeight(std::uint32_t) noexcept;
	};
} // namespace tudov

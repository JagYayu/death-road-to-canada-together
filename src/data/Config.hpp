#pragma once

#include "FileWatch.hpp"
#include "resource/ResourceType.hpp"
#include "util/Log.hpp"

#include "json.hpp"

#include <memory>
#include <optional>
#include <string>

namespace tudov
{
	class Config
	{
	  private:
		std::shared_ptr<Log> _log;
		nlohmann::json _config;
		filewatch::FileWatch<std::string> *_fileWatcher;
		std::optional<std::uint32_t> _propertiesID;

	  public:
		Config() noexcept;
		~Config() noexcept;

	  private:
		std::uint32_t GetPropertiesID() noexcept;

	  public:
		void Save() noexcept;
		void Load() noexcept;

		std::vector<std::string> GetMountBitmaps() noexcept;
		std::vector<std::string> GetMountDirectories() noexcept;
		std::unordered_map<std::string, ResourceType> GetMountFiles() noexcept;
		void GetDebugProfiling() noexcept;
		bool GetRenderBackend() noexcept;
		std::uint32_t GetWindowFramelimit() noexcept;
		std::string_view GetWindowTitle() noexcept;
		std::uint32_t GetWindowWidth() noexcept;
		std::uint32_t GetWindowHeight() noexcept;

		void SetWindowTitle(const std::string &) noexcept;
		void SetWindowWidth(std::uint32_t) noexcept;
		void SetWindowHeight(std::uint32_t) noexcept;

		std::int64_t GetCustom(std::string_view key, std::int64_t default_) noexcept;
		void SetCustom(std::string_view key, std::int64_t value) noexcept;
	};
} // namespace tudov

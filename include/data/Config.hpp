#pragma once

#include "FileWatch.hpp"
#include "resource/ResourceType.hpp"
#include "util/Log.hpp"

#include "json.hpp"

#include <optional>
#include <string>

namespace tudov
{
	class Config : public ILogProvider
	{
	  private:
		nlohmann::json _config;
		filewatch::FileWatch<std::string> *_fileWatcher;
		std::optional<std::uint32_t> _propertiesID;

	  public:
		explicit Config() noexcept;
		~Config() noexcept override;

	  private:
		std::uint32_t GetPropertiesID() noexcept;

	  public:
		Log &GetLog() noexcept override;

		void Save() noexcept;
		void Load() noexcept;

		std::vector<std::string> GetMountBitmaps() noexcept;
		std::vector<std::string> GetMountDirectories() noexcept;
		std::unordered_map<std::string, EResourceType> GetMountFiles() noexcept;
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

#pragma once

#include "FileWatch.hpp"
#include "resource/ResourceType.hpp"
#include "util/Log.hpp"

#include "json.hpp"

#include <optional>
#include <string>

namespace tudov
{
	class Config : private ILogProvider
	{
	  private:
		mutable nlohmann::json _config;
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

		std::vector<std::string> GetMountBitmaps() const noexcept;
		std::vector<std::string> GetMountDirectories() const noexcept;
		std::unordered_map<std::string, EResourceType> GetMountFiles() const noexcept;
		bool GetRenderBackend() const noexcept;
		std::uint32_t GetWindowFramelimit() const noexcept;
		std::string_view GetWindowTitle() const noexcept;
		std::uint32_t GetWindowWidth() const noexcept;
		std::uint32_t GetWindowHeight() const noexcept;

		void SetWindowTitle(const std::string &) noexcept;
		void SetWindowWidth(std::uint32_t) noexcept;
		void SetWindowHeight(std::uint32_t) noexcept;

		std::int64_t GetCustom(std::string_view key, std::int64_t default_) noexcept;
		void SetCustom(std::string_view key, std::int64_t value) noexcept;
	};
} // namespace tudov

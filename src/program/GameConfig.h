#pragma once

#include "IConfig.h"
#include "json.hpp"

#include <memory>
#include <string>

namespace tudov
{
	class GameConfig : public IConfig
	{
	private:
		nlohmann::json _config;

	public:
		static constexpr std::string_view file = std::string_view("config.json");

		GameConfig();
		~GameConfig();

		void Save() override;
		void Load() override;

		std::string GetWindowTitle() override;
		uint32_t GetWindowWidth() override;
		uint32_t GetWindowHeight() override;

		void SetWindowTitle(const std::string &) override;
		void SetWindowWidth(uint32_t) override;
		void SetWindowHeight(uint32_t) override;
	};
}

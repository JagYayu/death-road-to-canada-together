/**
 * @file mod/LuaAPI.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "ScriptEngine.hpp"

#include <functional>
#include <string_view>
#include <tuple>
#include <vector>

namespace tudov
{
	class Engine;

	struct ILuaAPI
	{
		using TInstallation = std::function<void(sol::state &)>;

		virtual ~ILuaAPI() noexcept = default;

		virtual bool RegisterInstallation(std::string_view key, const TInstallation &installation) = 0;
		virtual void Install(sol::state &lua, Context &context) = 0;
	};

	class LuaAPI : public ILuaAPI
	{
	  private:
		std::vector<std::tuple<std::string, TInstallation>> _installations;

	  public:
		explicit LuaAPI() noexcept = default;

		bool RegisterInstallation(std::string_view key, const TInstallation &installation) override;
		void Install(sol::state &lua, Context &context) override;
	};

	struct [[deprecated("DO NOT USE")]] ILuaAPIProvider
	{
		virtual ~ILuaAPIProvider() noexcept = default;

		virtual void ProvideLuaAPI(ILuaAPI &luaAPI) noexcept = 0;
	};
} // namespace tudov

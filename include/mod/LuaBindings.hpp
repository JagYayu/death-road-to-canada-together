/**
 * @file mod/LuaBindings.hpp
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

#include "sol/forward.hpp"

#include <functional>
#include <string_view>
#include <tuple>
#include <vector>

namespace tudov
{
	class Engine;

	/**
	 * Interface Class
	 * Provide lua bindings to engine.
	 */
	struct ILuaBindings
	{
		using TInstallation = std::function<void(sol::state &)>;

		virtual ~ILuaBindings() noexcept = default;

		virtual bool RegisterInstallation(std::string_view key, const TInstallation &installation) = 0;

		virtual void Install(sol::state &lua, Context &context) = 0;

		virtual const std::vector<std::string_view> &GetModGlobalsMigration() const noexcept = 0;
	};

	class LuaBindings : public ILuaBindings
	{
	  private:
		std::vector<std::tuple<std::string, TInstallation>> _installations;
		mutable std::vector<std::string_view> _modGlobalsMigration;

	  public:
		explicit LuaBindings() noexcept = default;

		bool RegisterInstallation(std::string_view key, const TInstallation &installation) override;
		void Install(sol::state &lua, Context &context) override;
		const std::vector<std::string_view> &GetModGlobalsMigration() const noexcept override;

	  private:
		void InstallEvent(sol::state &lua, Context &context) noexcept;
		void InstallMod(sol::state &lua, Context &context) noexcept;
		void InstallNetwork(sol::state &lua, Context &context) noexcept;
		void InstallScanCode(sol::state &lua, Context &context) noexcept;
		void InstallSystem(sol::state &lua, Context &context) noexcept;
	};

	struct [[deprecated("DO NOT USE")]] ILuaBindingsProvider
	{
		virtual ~ILuaBindingsProvider() noexcept = default;

		virtual void ProvideLuaBindings(ILuaBindings &luaBindings) noexcept = 0;
	};
} // namespace tudov

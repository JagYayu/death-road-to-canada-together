/**
 * @file resource/GameScripts.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Mod/LuaBindings.hpp"
#include "Program/EngineComponent.hpp"
#include "System/Log.hpp"

namespace tudov
{
	struct IGameScripts : public IEngineComponent, public ILuaBindings
	{
		virtual void RegisterEvents(IEventManager &eventManager) noexcept = 0;

		void Initialize() noexcept override;
	};

	class GameScripts final : public IGameScripts, public ILogProvider
	{
	  private:
		Context &_context;

	  public:
		explicit GameScripts(Context &context) noexcept;
		explicit GameScripts(const GameScripts &) noexcept = delete;
		explicit GameScripts(GameScripts &&) noexcept = delete;
		GameScripts &operator=(const GameScripts &) noexcept = delete;
		GameScripts &operator=(GameScripts &&) noexcept = delete;
		~GameScripts() noexcept = default;

		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;
		void Initialize() noexcept override;
		void Deinitialize() noexcept override;

		virtual void RegisterEvents(IEventManager &eventManager) noexcept override;
		virtual void ProvideLuaBindings(sol::state &lua, Context &context) override;
	};
} // namespace tudov

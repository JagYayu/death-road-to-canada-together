/**
 * @file program/EngineComponent.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "Context.hpp"

#include <cstdint>

namespace tudov
{
	class Context;
	struct AppEvent;
	struct ILuaBindings;

	/**
	 * As a core component of the engine, the subclass lifespan functions: "Initialize", "Reinitialize" are controlled by the engine.
	 */
	struct IEngineComponent : public IContextProvider
	{
		~IEngineComponent() noexcept override = default;

		virtual std::int32_t GetSortingSequence() noexcept;

		/**
		 * Pre initialize component contents, register delegate event handlers.
		 */
		virtual void PreInitialize() noexcept;

		/**
		 * Initialize component contents.
		 */
		virtual void Initialize() noexcept;

		/**
		 * Deinitialize component contents.
		 */
		virtual void Deinitialize() noexcept;

		/**
		 * Post Deinitialize component contents, unregister delegate event handlers.
		 */
		virtual void PostDeinitialize() noexcept;

		virtual bool HandleEvent(AppEvent &appEvent) noexcept;
		virtual void ProcessTick() noexcept;
		virtual void ProcessRender() noexcept;
		virtual void ProcessLoad() noexcept;

		[[deprecated]] virtual void ProvideLuaBindings(ILuaBindings &luaBindings) noexcept;
	};
} // namespace tudov

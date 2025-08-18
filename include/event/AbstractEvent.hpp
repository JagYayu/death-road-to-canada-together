/**
 * @file event/AbstractEvent.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "AddHandlerArgs.hpp"
#include "program/Context.hpp"
#include "util/Definitions.hpp"

namespace tudov
{
	struct IEventManager;
	class RuntimeEvent;
	class Context;

	class AbstractEvent : public IContextProvider
	{
	  protected:
		EventID _id;
		ScriptID _scriptID;

	  public:
		IEventManager &eventManager;

	  protected:
		explicit AbstractEvent(IEventManager &eventManager, EventID eventID, ScriptID scriptID = false);
		virtual ~AbstractEvent() noexcept;

	  public:
		Context &GetContext() noexcept override;

		/**
		 * Check if this event is valid.
		 */
		void IsValid() const noexcept;

		/**
		 * Get the event's id.
		 */
		EventID GetID() const noexcept;

		/**
		 * Get the event's creator script id, 0 if it is a builtin event.
		 */
		ScriptID GetScriptID() const noexcept;

		virtual void Add(const AddHandlerArgs &args) = 0;
	};
} // namespace tudov

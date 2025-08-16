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
#include "util/Definitions.hpp"

namespace tudov
{
	struct IEventManager;
	class RuntimeEvent;

	class AbstractEvent
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
		void IsValid() const noexcept;
		EventID GetID() const noexcept;
		ScriptID GetScriptID() const noexcept;

		virtual void Add(const AddHandlerArgs &args) = 0;
	};
} // namespace tudov

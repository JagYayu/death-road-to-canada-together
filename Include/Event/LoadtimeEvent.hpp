/**
 * @file event/LoadtimeEvent.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "AbstractEvent.hpp"
#include "AddHandlerArgs.hpp"
#include "OverrideHandlerArgs.hpp"
#include "Util/Definitions.hpp"

namespace tudov
{
	class LoadtimeEvent : public AbstractEvent
	{
	  protected:
		using Operation = std::variant<AddHandlerArgs, OverrideHandlerArgs>;

		bool _built;
		std::vector<std::string> _orders;
		std::vector<EventHandleKey> _keys;
		std::vector<Operation> _operations;
		std::string _scriptTraceback;

	  public:
		explicit LoadtimeEvent(IEventManager &eventManager, EventID eventID, ScriptID scriptID) noexcept;
		explicit LoadtimeEvent(IEventManager &eventManager, EventID eventID, ScriptID scriptID, const std::vector<std::string> &orders, const std::vector<EventHandleKey> &keys) noexcept;
		explicit LoadtimeEvent(const LoadtimeEvent &) noexcept = default;
		explicit LoadtimeEvent(LoadtimeEvent &&) noexcept = default;
		LoadtimeEvent &operator=(const LoadtimeEvent &) noexcept = delete;
		LoadtimeEvent &operator=(LoadtimeEvent &&) noexcept = delete;
		~LoadtimeEvent() noexcept override;

		virtual void Add(const AddHandlerArgs &handler) override;

		bool IsBuilt() const noexcept;
		/*
		 * return true if event already been built.
		 */
		bool TryBuild(ScriptID scriptID, const std::vector<std::string> &orders, const std::vector<EventHandleKey> &keys) noexcept;
		RuntimeEvent ToRuntime() const;
		std::string_view GetScriptTraceBack() const noexcept;
	};
} // namespace tudov

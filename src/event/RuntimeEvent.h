#pragma once

#include "AbstractEvent.h"
#include "debug/EventProfiler.h"
#include "util/Defs.h"
#include "util/Log.h"

#include <optional>
#include <sol/forward.hpp>
#include <sol/types.hpp>

#include <string>
#include <variant>
#include <vector>

namespace tudov
{
	class EventProfiler;
	class ScriptProvider;

	class RuntimeEvent : public AbstractEvent
	{
		using InvocationCache = std::vector<EventHandler::Function>;

		struct Profile
		{
			bool traceHandlers;
			EventProfiler eventProfiler;

			template <typename... TArgs>
			explicit Profile(bool traceHandlers, TArgs &&...args) noexcept
			    : traceHandlers(traceHandlers),
			      eventProfiler(Move<TArgs>(args)...)
			{
			}
		};

	  private:
		SharedPtr<Log> _log;
		UniquePtr<Profile> _profile;
		bool _handlersSortedCache;
		std::optional<InvocationCache> _invocationCache;
		std::unordered_map<EventHandler::Key, InvocationCache, EventHandler::Key::Hash, EventHandler::Key::Equal> _invocationCaches;
		std::vector<std::string> _orders;
		std::unordered_set<EventHandler::Key, EventHandler::Key::Hash, EventHandler::Key::Equal> _keys;
		std::vector<EventHandler> _handlers;

	  public:
		explicit RuntimeEvent(EventManager &eventManager, EventID eventID, const std::vector<std::string> &orders = {""}, const std::unordered_set<EventHandler::Key, EventHandler::Key::Hash, EventHandler::Key::Equal> &keys = {}, ScriptID scriptID = false);

	  private:
		void ClearCaches();
		void ClearScriptHandlersImpl(std::function<bool(const EventHandler &)> pred);

		void InvokeFunction() noexcept;

	  protected:
		std::vector<EventHandler> &GetSortedHandlers();

	  public:
		std::optional<Reference<RuntimeEvent::Profile>> GetProfile() const noexcept;
		void EnableProfiler(bool traceHandlers) noexcept;
		void DisableProfiler() noexcept;

		void Add(const AddHandlerArgs &args) override;

		void Invoke(const sol::object &args = sol::lua_nil, std::optional<EventHandler::Key> key = nullopt);
		void InvokeUncached(const sol::object &args = sol::lua_nil, std::optional<EventHandler::Key> key = nullopt);

		void ClearInvalidScriptsHandlers(const ScriptProvider &scriptProvider);
		void ClearScriptsHandlers();
	};
} // namespace tudov

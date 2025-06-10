#pragma once

#include "DelegateEvent.hpp"
#include "LoadtimeEvent.h"
#include "RuntimeEvent.h"
#include "util/Defs.h"
#include "util/StringUtils.hpp"

#include "sol/sol.hpp"
#include <string_view>

namespace tudov
{
	class Engine;
	class ScriptLoader;

	class EventManager
	{
	  private:
		Engine &_engine;

		UnorderedMap<StringView, LoadtimeEvent, StringSVHash, StringSVEqual> _loadtimeEvents;
		UnorderedMap<StringView, RuntimeEvent, StringSVHash, StringSVEqual> _runtimeEvents;
		Vector<Reference<RuntimeEvent>> _staticEvents;

		bool _initialized;
		DelegateEvent<>::HandlerID _onPreLoadScriptHandlerID;

	  public:
		RuntimeEvent update;
		RuntimeEvent render;

		explicit EventManager(Engine &engine);
		~EventManager();

	  private:
		void OnPreLoadScript(const StringView &scriptName);
		void OnPreLoadScripts();
		void OnScriptsLoaded();

	  public:
		void Initialize();

		Optional<Reference<AbstractEvent>> TryGetRegistryEvent(const StringView &eventName);
	};
} // namespace tudov

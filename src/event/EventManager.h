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
	class ScriptEngine;

	class EventManager
	{
	  private:
		UnorderedMap<StringView, LoadtimeEvent, StringSVHash, StringSVEqual> _loadtimeEvents;
		UnorderedMap<StringView, RuntimeEvent, StringSVHash, StringSVEqual> _runtimeEvents;
		Vector<Reference<RuntimeEvent>> _staticEvents;

		bool _initialized;
		DelegateEvent<>::HandlerID _onPreLoadScriptHandlerID;

	  public:
		Engine &engine;

		RuntimeEvent update;
		RuntimeEvent render;

	  public:
		explicit EventManager(Engine &engine);
		~EventManager();

	  private:
		void OnPreLoadScript(StringView scriptName);
		void OnPreLoadScripts();
		void OnScriptsLoaded();

	  public:
		void Initialize();

		void RegisterScriptGlobal(ScriptEngine &lua);

		Optional<Reference<AbstractEvent>> TryGetRegistryEvent(StringView eventName);
	};
} // namespace tudov

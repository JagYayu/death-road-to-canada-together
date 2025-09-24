/**
 * @file event/CoreEvents.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <memory>

namespace tudov
{
	class RuntimeEvent;

	struct ICoreEvents
	{
		// Tick events.

		[[nodiscard]] virtual RuntimeEvent &TickLoad() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &TickUpdate() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &TickRender() noexcept = 0;

		// Keyboard events.

		[[nodiscard]] virtual RuntimeEvent &KeyboardPress() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &KeyboardRelease() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &KeyboardRepeat() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &KeyboardAdded() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &KeyboardRemoved() noexcept = 0;

		// Mouse events.

		[[nodiscard]] virtual RuntimeEvent &MouseMotion() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &MouseButtonDown() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &MouseButtonUp() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &MouseWheel() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &MouseAdded() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &MouseRemoved() noexcept = 0;

		// Window events.

		[[nodiscard]] virtual RuntimeEvent &WindowResize() noexcept = 0;

		// Network events.

		/**
		 * Client observer.
		 * Invoke when a client connected to current server.
		 */
		[[nodiscard]] virtual RuntimeEvent &ClientConnect() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &ClientDisconnect() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &ClientMessage() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &ServerConnect() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &ServerDisconnect() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &ServerHost() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &ServerMessage() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &ServerShutdown() noexcept = 0;

		// Debug events.

		/**
		 * Provide debug elements / commands from lua modules.
		 */
		[[nodiscard]] virtual RuntimeEvent &DebugProvide() noexcept = 0;
		/**
		 * Generate snapshot tables from lua modules to inspect data and memory.
		 */
		[[nodiscard]] virtual RuntimeEvent &DebugSnapshot() noexcept = 0;

		// Mod events.

		/**
		 * Invoke when a script attempts to access a non-exist global variable.
		 */
		[[nodiscard]] virtual RuntimeEvent &ScriptGlobalIndex() noexcept = 0;

		/**
		 * Invoke when Script Loader completes a full script loads.
		 */
		[[nodiscard]] virtual RuntimeEvent &ScriptsLoaded() noexcept = 0;

		/**
		 * Invoke when a script was unloaded.
		 */
		[[nodiscard]] virtual RuntimeEvent &ScriptUnload() noexcept = 0;

		// [[nodiscard]] virtual RuntimeEvent &LocalizationUpdateTexts() noexcept = 0;

		// Unused events.

		[[deprecated("Server validation logic should be written in scripts")]]
		virtual RuntimeEvent &ServerAuthenticate() noexcept = 0;
	};

	class EventManager;
	struct IEventManager;

	class CoreEvents : public ICoreEvents
	{
		friend EventManager;

	  private:
		using TRuntimeEvent = std::shared_ptr<RuntimeEvent>;

	  private:
		TRuntimeEvent _clientConnect;
		TRuntimeEvent _clientDisconnect;
		TRuntimeEvent _clientMessage;
		TRuntimeEvent _debugProvide;
		TRuntimeEvent _debugSnapshot;
		TRuntimeEvent _keyboardPress;
		TRuntimeEvent _keyboardRepeat;
		TRuntimeEvent _keyboardRelease;
		TRuntimeEvent _keyboardAdded;
		TRuntimeEvent _keyboardRemoved;
		TRuntimeEvent _mouseMotion;
		TRuntimeEvent _mouseButtonDown;
		TRuntimeEvent _mouseButtonUp;
		TRuntimeEvent _mouseWheel;
		TRuntimeEvent _mouseAdded;
		TRuntimeEvent _mouseRemoved;
		TRuntimeEvent _scriptGlobalIndex;
		TRuntimeEvent _scriptUnload;
		TRuntimeEvent _scriptsLoaded;
		TRuntimeEvent _serverAuthenticate;
		TRuntimeEvent _serverConnect;
		TRuntimeEvent _serverDisconnect;
		TRuntimeEvent _serverHost;
		TRuntimeEvent _serverMessage;
		TRuntimeEvent _serverShutdown;
		TRuntimeEvent _tickLoad;
		TRuntimeEvent _tickRender;
		TRuntimeEvent _tickUpdate;
		TRuntimeEvent _windowResize;

	  public:
		explicit CoreEvents(EventManager &eventManager) noexcept;

		[[nodiscard]] RuntimeEvent &ClientConnect() noexcept override;
		[[nodiscard]] RuntimeEvent &ClientDisconnect() noexcept override;
		[[nodiscard]] RuntimeEvent &ClientMessage() noexcept override;
		[[nodiscard]] RuntimeEvent &DebugProvide() noexcept override;
		[[nodiscard]] RuntimeEvent &DebugSnapshot() noexcept override;
		[[nodiscard]] RuntimeEvent &KeyboardPress() noexcept override;
		[[nodiscard]] RuntimeEvent &KeyboardRepeat() noexcept override;
		[[nodiscard]] RuntimeEvent &KeyboardRelease() noexcept override;
		[[nodiscard]] RuntimeEvent &KeyboardAdded() noexcept override;
		[[nodiscard]] RuntimeEvent &KeyboardRemoved() noexcept override;
		[[nodiscard]] RuntimeEvent &MouseMotion() noexcept override;
		[[nodiscard]] RuntimeEvent &MouseButtonDown() noexcept override;
		[[nodiscard]] RuntimeEvent &MouseButtonUp() noexcept override;
		[[nodiscard]] RuntimeEvent &MouseWheel() noexcept override;
		[[nodiscard]] RuntimeEvent &MouseAdded() noexcept override;
		[[nodiscard]] RuntimeEvent &MouseRemoved() noexcept override;
		[[nodiscard]] RuntimeEvent &ScriptGlobalIndex() noexcept override;
		[[nodiscard]] RuntimeEvent &ScriptUnload() noexcept override;
		[[nodiscard]] RuntimeEvent &ScriptsLoaded() noexcept override;
		[[nodiscard]] RuntimeEvent &ServerAuthenticate() noexcept override;
		[[nodiscard]] RuntimeEvent &ServerConnect() noexcept override;
		[[nodiscard]] RuntimeEvent &ServerDisconnect() noexcept override;
		[[nodiscard]] RuntimeEvent &ServerHost() noexcept override;
		[[nodiscard]] RuntimeEvent &ServerMessage() noexcept override;
		[[nodiscard]] RuntimeEvent &ServerShutdown() noexcept override;
		[[nodiscard]] RuntimeEvent &TickLoad() noexcept override;
		[[nodiscard]] RuntimeEvent &TickRender() noexcept override;
		[[nodiscard]] RuntimeEvent &TickUpdate() noexcept override;
		[[nodiscard]] RuntimeEvent &WindowResize() noexcept override;
	};
} // namespace tudov

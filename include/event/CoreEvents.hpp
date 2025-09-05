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

		[[nodiscard]] virtual RuntimeEvent &KeyDown() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &KeyRepeat() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &KeyUp() noexcept = 0;

		// Mouse events.

		[[nodiscard]] virtual RuntimeEvent &MouseMove() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &MouseButtonDown() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &MouseButtonUp() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &MouseWheel() noexcept = 0;

		// Network events.

		/**
		 * Client observer.
		 * Invoke when a client connected to current server.
		 */
		[[nodiscard]] virtual RuntimeEvent &ClientConnect() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &ClientDisconnect() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &ClientMessage() noexcept = 0;
		// unused
		[[nodiscard]] virtual RuntimeEvent &ServerAuthenticate() noexcept = 0;
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

		// Other events.

		[[nodiscard]] virtual RuntimeEvent &ModGlobalsIndex() noexcept = 0;
		[[nodiscard]] virtual RuntimeEvent &ScriptsLoaded() noexcept = 0;

		// [[nodiscard]] virtual RuntimeEvent &LocalizationUpdateTexts() noexcept = 0;
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
		TRuntimeEvent _keyDown;
		TRuntimeEvent _keyRepeat;
		TRuntimeEvent _keyUp;
		TRuntimeEvent _modGlobalsIndex;
		TRuntimeEvent _mouseMove;
		TRuntimeEvent _mouseButtonDown;
		TRuntimeEvent _mouseButtonUp;
		TRuntimeEvent _mouseWheel;
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

	  public:
		explicit CoreEvents(EventManager &eventManager) noexcept;

		[[nodiscard]] RuntimeEvent &ClientConnect() noexcept override;
		[[nodiscard]] RuntimeEvent &ClientDisconnect() noexcept override;
		[[nodiscard]] RuntimeEvent &ClientMessage() noexcept override;
		[[nodiscard]] RuntimeEvent &DebugProvide() noexcept override;
		[[nodiscard]] RuntimeEvent &DebugSnapshot() noexcept override;
		[[nodiscard]] RuntimeEvent &KeyDown() noexcept override;
		[[nodiscard]] RuntimeEvent &KeyRepeat() noexcept override;
		[[nodiscard]] RuntimeEvent &KeyUp() noexcept override;
		[[nodiscard]] RuntimeEvent &MouseMove() noexcept override;
		[[nodiscard]] RuntimeEvent &MouseButtonDown() noexcept override;
		[[nodiscard]] RuntimeEvent &MouseButtonUp() noexcept override;
		[[nodiscard]] RuntimeEvent &MouseWheel() noexcept override;
		[[nodiscard]] RuntimeEvent &ModGlobalsIndex() noexcept override;
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
	};
} // namespace tudov

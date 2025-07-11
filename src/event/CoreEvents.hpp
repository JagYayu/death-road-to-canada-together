#pragma once

#include "RuntimeEvent.hpp"

namespace tudov
{
	struct ICoreEvents
	{
		using TRuntimeEvent = std::shared_ptr<RuntimeEvent>;

		// Tick events.

		[[nodiscard]] virtual TRuntimeEvent TickUpdate() noexcept = 0;
		[[nodiscard]] virtual TRuntimeEvent TickRender() noexcept = 0;

		// Keyboard events.

		[[nodiscard]] virtual TRuntimeEvent KeyDown() noexcept = 0;
		[[nodiscard]] virtual TRuntimeEvent KeyUp() noexcept = 0;

		// Mouse events.

		[[nodiscard]] virtual TRuntimeEvent MouseMove() noexcept = 0;
		[[nodiscard]] virtual TRuntimeEvent MouseButtonDown() noexcept = 0;
		[[nodiscard]] virtual TRuntimeEvent MouseButtonUp() noexcept = 0;
		[[nodiscard]] virtual TRuntimeEvent MouseWheel() noexcept = 0;

		// Network events.

		/*
		 * Client observer. Invoke when a client connected to current server.
		 */
		[[nodiscard]] virtual TRuntimeEvent ClientConnect() noexcept = 0;
		[[nodiscard]] virtual TRuntimeEvent ClientDisconnect() noexcept = 0;
		[[nodiscard]] virtual TRuntimeEvent ClientMessage() noexcept = 0;
		[[nodiscard]] virtual TRuntimeEvent ServerConnect() noexcept = 0;
		[[nodiscard]] virtual TRuntimeEvent ServerDisconnect() noexcept = 0;
		[[nodiscard]] virtual TRuntimeEvent ServerMessage() noexcept = 0;

		// Other events.
	};

	class EventManager;
	struct IEventManager;

	class CoreEvents : public ICoreEvents
	{
		friend EventManager;

	  private:
		TRuntimeEvent _clientConnect;
		TRuntimeEvent _clientDisconnect;
		TRuntimeEvent _clientMessage;
		TRuntimeEvent _keyDown;
		TRuntimeEvent _keyUp;
		TRuntimeEvent _mouseMove;
		TRuntimeEvent _mouseButtonDown;
		TRuntimeEvent _mouseButtonUp;
		TRuntimeEvent _mouseWheel;
		TRuntimeEvent _serverConnect;
		TRuntimeEvent _serverDisconnect;
		TRuntimeEvent _serverMessage;
		TRuntimeEvent _tickRender;
		TRuntimeEvent _tickUpdate;

	  public:
		explicit CoreEvents(EventManager &eventManager) noexcept;

		[[nodiscard]] TRuntimeEvent ClientConnect() noexcept override;
		[[nodiscard]] TRuntimeEvent ClientDisconnect() noexcept override;
		[[nodiscard]] TRuntimeEvent ClientMessage() noexcept override;
		[[nodiscard]] TRuntimeEvent KeyDown() noexcept override;
		[[nodiscard]] TRuntimeEvent KeyUp() noexcept override;
		[[nodiscard]] TRuntimeEvent MouseMove() noexcept override;
		[[nodiscard]] TRuntimeEvent MouseButtonDown() noexcept override;
		[[nodiscard]] TRuntimeEvent MouseButtonUp() noexcept override;
		[[nodiscard]] TRuntimeEvent MouseWheel() noexcept override;
		[[nodiscard]] TRuntimeEvent ServerConnect() noexcept override;
		[[nodiscard]] TRuntimeEvent ServerDisconnect() noexcept override;
		[[nodiscard]] TRuntimeEvent ServerMessage() noexcept override;
		[[nodiscard]] TRuntimeEvent TickRender() noexcept override;
		[[nodiscard]] TRuntimeEvent TickUpdate() noexcept override;
	};
} // namespace tudov

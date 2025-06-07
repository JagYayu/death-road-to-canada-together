#pragma once

namespace tudov
{
	class GameEngine;

	class EventManager
	{
	private:
		GameEngine &_gameEngine;

	public:
		explicit EventManager(GameEngine &gameEngine);
	};
}

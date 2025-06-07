#pragma once

#include <memory>

namespace sol
{
	class state;
}

namespace tudov
{
	class GameEngine;

	class ScriptEngine
	{
	private:
		GameEngine* _gameEngine;
		std::unique_ptr<sol::state> _lua;

	public:
		ScriptEngine(GameEngine* gameEngine);
	};
}

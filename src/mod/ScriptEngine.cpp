#include "ScriptEngine.h"

#include <sol/sol.hpp>

using namespace tudov;

ScriptEngine::ScriptEngine(GameEngine *gameEngine)
	: _gameEngine(gameEngine),
	  _lua(std::make_unique<sol::state>())
{
}
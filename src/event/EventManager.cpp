#include "EventManager.h"

using namespace tudov;

EventManager::EventManager(GameEngine &gameEngine)
    : _gameEngine(gameEngine)
{
}

void EventManager::Initialize(sol::state &lua)
{
	auto &&usertype = lua.new_usertype<EventManager>("EventManager");

	// usertype["add"] = [&](std::string_view name,  std::function func) {

	// };
}

// TOOD
void EventManager::ClearScriptHandlers();
void EventManager::ClearScriptHandlers(std::string_view);

void EventManager::HandleRuntimeTransition();

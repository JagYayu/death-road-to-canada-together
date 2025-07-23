#include "scripts/GameScripts.hpp"

using namespace tudov;

GameScripts::GameScripts(Context &context) noexcept
    : _context(context)
{
}

Context &GameScripts::GetContext() noexcept
{
	return _context;
}

void GameScripts::Initialize() noexcept
{
}

void GameScripts::Deinitialize() noexcept
{
}

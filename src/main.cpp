#include "program/GameEngine.h"

#include <memory>

using namespace tudov;

int main(int argc, char **args)
{
	auto &&engine = std::make_unique<GameEngine>();
	engine->Initialize(MainArgs(argc, args));
	engine->Run();

	return 0;
}

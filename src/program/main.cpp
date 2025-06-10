#include "program/Engine.h"

int main(int argc, char **args)
{
	tudov::Engine engine;
	engine.Run(tudov::MainArgs{argc, args});
	return 0;
}

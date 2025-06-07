#pragma once

#include "program/MainArgs.hpp"
#include "util/Micros.h"

namespace tudov
{
	INTERFACE IEngine
	{
	public:
		virtual void Initialize(const MainArgs &args) = 0;
		virtual void Run() = 0;
		virtual void Quit() = 0;
	};
}

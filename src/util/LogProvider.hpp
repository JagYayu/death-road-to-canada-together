#pragma once

#include "util/Log.h"

namespace tudov
{
	class LogProvider
	{
	public:
		Log log;

		explicit LogProvider(const std::string &module)
			: log(module)
		{
		}
	};
}
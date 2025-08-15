#pragma once

#include "util/Micros.hpp"

namespace tudov
{
	struct CrashReporter
	{
		TE_STATIC_CLASS(CrashReporter);

		static void InitializeCrashReporter() noexcept;
	};
} // namespace tudov

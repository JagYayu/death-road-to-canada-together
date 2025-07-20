#pragma once

#include <cstddef>

namespace tudov
{
	class Memory
	{
		static std::size_t GetHeapUsage() noexcept;
	};
}
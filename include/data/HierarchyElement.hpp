#pragma once

namespace tudov
{
	enum class EHierarchyElement : char
	{
		All = -1,
		None = 0,
		Data = 1 << 0,
		Directory = 1 << 1,
	};
} // namespace tudov

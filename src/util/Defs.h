#pragma once

#include "Micros.h"

#include <array>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <deque>
#include <format>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace tudov
{
	using EventID = std::uint64_t;
	using ResourceID = std::uint64_t;
	using ScriptID = std::uint64_t;

	using ImageID = ResourceID;
	using TextureID = ResourceID;
	using FontID = ResourceID;
} // namespace tudov
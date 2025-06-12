#pragma once

#include "Log.h"
#include "util/Defs.h"

#include "sol/sol.hpp"
#include "sol/types.hpp"

namespace tudov
{
	struct LuaTableHash
	{
		size_t operator()(const sol::table &t) const noexcept
		{
			return std::hash<const void *>{}(t.pointer());
		}
	};

	struct LuaTableEqual
	{
		bool operator()(const sol::table &lhs, const sol::table &rhs) const noexcept
		{
			return lhs.pointer() == rhs.pointer();
		}
	};

	inline StringView GetLuaTypeStringView(sol::type luaType) noexcept
	{
		switch (luaType)
		{
		case sol::type::none:
		case sol::type::lua_nil:
			return "nil";
		case sol::type::string:
			return "string";
		case sol::type::number:
			return "number";
		case sol::type::thread:
			return "thread";
		case sol::type::boolean:
			return "boolean";
		case sol::type::function:
			return "function";
		case sol::type::userdata:
		case sol::type::lightuserdata:
			return "userdata";
		case sol::type::table:
			return "table";
		default:
			return "unknown";
		}
	}

	template <typename T>
	concept TypenameMapLike = requires {
		typename T::key_type;
		typename T::mapped_type;
	} && (std::is_same_v<T, std::map<typename T::key_type, typename T::mapped_type, typename T::key_compare, typename T::allocator_type>> || std::is_same_v<T, std::unordered_map<typename T::key_type, typename T::mapped_type, typename T::hasher, typename T::key_equal, typename T::allocator_type>>);

	template <TypenameMapLike TMap, typename TPred>
	void MapEraseIf(TMap &map, const TPred &pred)
	{
		MapEraseIf(map, pred, [](const auto &, const auto &) {});
	}

	template <TypenameMapLike TMap, typename TPred, typename TCallback>
	[[deprecated("nah")]]
	void MapEraseIf(TMap &map, const TPred &pred, const TCallback &callback)
	{
		for (auto it = map.begin(); it != map.end();)
		{
			auto &&key = it->first;
			auto &&value = it->second;
			if (pred(key, value))
			{
				callback(key, value);
				it = map.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	FORCEINLINE void UnhandledCppException(SharedPtr<Log> &log, const std::exception &e)
	{
		log->Fatal("Unhandled C++ exception occurred: {}", e.what());
	}
} // namespace tudov

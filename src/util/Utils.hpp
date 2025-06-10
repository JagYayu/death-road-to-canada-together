#pragma once

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
			return "userdata";
		case sol::type::table:
			return "table";
		case sol::type::none:
			return "sol::type::none";
		case sol::type::lightuserdata:
			return "sol::type::lightuserdata";
		case sol::type::poly:
			return "sol::type::poly";
		default:
			return "sol::type";
		}
	}

	template <typename T>
	concept TypenameMapLike = requires {
		typename T::key_type;
		typename T::mapped_type;
	} && (std::is_same_v<T, std::map<typename T::key_type, typename T::mapped_type, typename T::key_compare, typename T::allocator_type>> || std::is_same_v<T, std::unordered_map<typename T::key_type, typename T::mapped_type, typename T::hasher, typename T::key_equal, typename T::allocator_type>>);

	template <TypenameMapLike TMap, typename TPred>
	void MapRemoveIf(TMap &map, TPred pred)
	{
		for (auto it = map.begin(); it != map.end();)
		{
			if (pred(*it))
			{
				it = map.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
} // namespace tudov

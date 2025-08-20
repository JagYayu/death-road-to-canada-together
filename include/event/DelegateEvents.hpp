/**
 * @file event/DelegateEvents.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "DelegateEvent.hpp"
#include "util/Micros.hpp"

#include <stdexcept>
#include <type_traits>
#include <unordered_map>

namespace tudov
{
	template <typename... TArgs>
	class DelegateEvent;

	/**
	 * Not implement yet
	 */
	class DelegateEvents
	{
		TE_STATIC_CLASS(DelegateEvents);

		struct Entry
		{
			void *delegateEvent;
			const char *typeRawName;
		};

		// static std::unordered_map<void *, Entry> delegateEvents;

	  public:
		// template <typename... TArgs>
		// TE_FORCEINLINE static DelegateEvent<TArgs...> *Register(void *object, TArgs &&...args)
		// {
		// 	auto *event = new DelegateEvent<TArgs...>(std::forward<TArgs>(args)...);
		// 	auto result = delegateEvents.try_emplace(object, event, typeid(std::remove_pointer_t<decltype(event)>).raw_name());
		// 	if (!result.second) [[unlikely]]
		// 	{
		// 		return nullptr;
		// 	}
		// 	return result.first->second;
		// }

		// template <typename... TArgs>
		// TE_FORCEINLINE static bool Unregister(void *object) noexcept
		// {
		// 	return delegateEvents.erase(object);
		// }

		// template <typename... TArgs>
		// TE_FORCEINLINE static DelegateEvent<TArgs...> *Get(void *object, TArgs &&...args)
		// {
		// 	auto it = delegateEvents.find(object);
		// 	if (it == delegateEvents.end())
		// 	{
		// 		return nullptr;
		// 	}

		// 	if (typeid(DelegateEvent<TArgs...>).raw_name() != it->second.typeRawName)
		// 	{
		// 		throw std::runtime_error("Delegate event type mismatch");
		// 	}

		// 	return static_cast<DelegateEvent<TArgs...> *>(it->second);
		// }
	};
} // namespace tudov

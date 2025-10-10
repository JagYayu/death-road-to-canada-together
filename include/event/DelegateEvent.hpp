/**
 * @file event/DelegateEvent.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Util/Definitions.hpp"

#include <functional>

namespace tudov
{
	/**
	 * @brief Provided for internal use by the C++ tudov engine.
	 */
	template <typename... TArgs>
	class DelegateEvent
	{
	  public:
		using HandlerType = std::function<void(TArgs...)>;
		using HandlerID = DelegateEventHandlerID;

	  private:
		std::vector<std::pair<HandlerID, HandlerType>> _handlers;
		HandlerID _handlerID;

	  public:
		DelegateEvent()
		    : _handlers(),
		      _handlerID(0)
		{
		}
		explicit DelegateEvent(const DelegateEvent &) noexcept = delete;
		explicit DelegateEvent(DelegateEvent &&) noexcept = delete;
		DelegateEvent &operator=(const DelegateEvent &) noexcept = delete;
		DelegateEvent &operator=(DelegateEvent &&) noexcept = delete;

		HandlerID operator+=(const HandlerType &handler)
		{
			++_handlerID;
			_handlers.emplace_back(_handlerID, handler);
			return _handlerID;
		}

		void operator-=(HandlerID id)
		{
			auto it = std::find_if(_handlers.begin(), _handlers.end(), [this, id](const auto &pair)
			{
				return pair.first == id;
			});

			if (it != _handlers.end())
			{
				_handlers.erase(it);
			}
		}

		template <typename TObj>
		HandlerID Bind(TObj *obj, void (TObj::*func)(TArgs...))
		{
			return *this += [obj, func](TArgs... args)
			{
				(obj->*func)(args...);
			};
		}

		template <typename... UArgs>
		void Invoke(UArgs &&...args) const
		{
			for (const auto &handler : _handlers)
			{
				if (handler.second)
				{
					handler.second(std::forward<UArgs>(args)...);
				}
			}
		}

		template <typename... U>
		void operator()(U &&...args) const
		{
			Invoke(std::forward<U>(args)...);
		}
	};
} // namespace tudov

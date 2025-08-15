#pragma once

#include "util/Definitions.hpp"

#include <functional>

namespace tudov
{
	/**
	 * @brief Provided for internal use by the C++ tudov engine.
	 */
	template <typename... Args>
	class DelegateEvent
	{
	  public:
		using HandlerType = std::function<void(Args...)>;
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
		HandlerID Bind(TObj *obj, void (TObj::*func)(Args...))
		{
			return *this += [obj, func](Args... args)
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

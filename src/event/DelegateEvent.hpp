#pragma once

#include "util/Defs.h"

namespace tudov
{
	template <typename... Args>
	class DelegateEvent
	{
	  public:
		using HandlerType = Function<void(Args...)>;
		using HandlerID = size_t;

	  private:
		Vector<Pair<HandlerID, HandlerType>> _handlers;
		HandlerID _handlerID;

	  public:
		DelegateEvent()
		    : _handlers(),
		      _handlerID(0)
		{
		}

		HandlerID operator+=(const HandlerType &handler)
		{
			_handlers.emplace_back(_handlerID, handler);
			return _handlerID++;
		}

		void operator-=(HandlerID id)
		{
			auto it = FindIf(_handlers.begin(), _handlers.end(), [&](const auto &pair)
			{
				return pair.first == id;
			});

			if (it != _handlers.end())
			{
				_handlers.erase(it);
			}
		}

		template <typename T>
		HandlerID Bind(T *obj, void (T::*func)(Args...))
		{
			return *this += [obj, func](Args... args)
			{
				(obj->*func)(args...);
			};
		}

		void Invoke(Args &&...args) const
		{
			for (const auto &handler : _handlers)
			{
				if (handler.second)
				{
					handler.second(Forward<Args>(args)...);
				}
			}
		}

		void operator()(Args... args) const
		{
			Invoke(Forward<Args>(args)...);
		}
	};
} // namespace tudov
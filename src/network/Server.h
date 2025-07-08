#pragma once

#include "SocketType.h"
#include "program/Context.h"

namespace tudov
{
	struct IServer : public ISocketTypeProvider, public IContextProvider
	{
	  public:
		virtual void Host() noexcept = 0;
		virtual void Shutdown() noexcept = 0;
	};
} // namespace tudov

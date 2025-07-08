#pragma once

#include "SocketType.h"
#include "program/Context.h"

#include <string_view>

namespace tudov
{
	struct IClient : public ISocketTypeProvider, public IContextProvider
	{
		virtual void Connect(std::string_view address) = 0;
		virtual void Disconnect() = 0;
	};
} // namespace tudov

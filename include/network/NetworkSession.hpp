/**
 * @file network/NetworkComponent.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "SocketType.hpp"
#include "program/Context.hpp"

namespace tudov
{
	enum class ESessionEvent : std::uint8_t
	{
		Connect,
		Receive,
		Disconnect,
	};

	struct INetworkManager;

	struct INetworkSession : public ISocketTypeProvider, public IContextProvider
	{
		virtual INetworkManager &GetNetworkManager() noexcept = 0;
		virtual bool Update() = 0;

		Context &GetContext() noexcept override;
	};
} // namespace tudov

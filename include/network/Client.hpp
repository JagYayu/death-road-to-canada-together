/**
 * @file network/Client.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "NetworkComponent.hpp"

#include <exception>
#include <string_view>

namespace tudov
{
	struct IClient : public INetworkComponent
	{
		struct ConnectArgs
		{
			virtual ~ConnectArgs() noexcept = default;

			std::string_view password = "";
		};

		virtual ~IClient() noexcept override = default;

		virtual void Connect(const ConnectArgs &address) = 0;
		virtual void Disconnect() = 0;
		virtual bool IsConnecting() noexcept = 0;
		virtual bool IsConnected() noexcept = 0;
		virtual void SendReliable(std::string_view data) = 0;
		virtual void SendUnreliable(std::string_view data) = 0;

		inline bool TryDisconnect() noexcept
		{
			if (IsConnected())
			{
				try
				{
					Disconnect();
					return true;
				}
				catch (std::exception &e)
				{
				};
			}
			return false;
		}

		inline bool IsDisconnected() noexcept
		{
			return !IsConnected() && !IsConnecting();
		}
	};
} // namespace tudov

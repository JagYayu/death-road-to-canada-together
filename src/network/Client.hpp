#pragma once

#include "NetworkComponent.hpp"

namespace tudov
{
	struct IClient : public INetworkComponent
	{
		struct ConnectArgs
		{
			virtual ~ConnectArgs() noexcept = default;
		};

		virtual void Connect(const ConnectArgs &address) = 0;
		virtual void Disconnect() = 0;
		virtual bool IsConnecting() noexcept = 0;
		virtual bool IsConnected() noexcept = 0;

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

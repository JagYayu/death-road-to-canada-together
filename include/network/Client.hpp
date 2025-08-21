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
#include "util/Micros.hpp"

#include "sol/forward.hpp"

#include <exception>
#include <string_view>

namespace tudov
{
	class LuaAPI;

	struct IClient : public INetworkComponent
	{
		friend LuaAPI;

		struct ConnectArgs
		{
			virtual ~ConnectArgs() noexcept = default;

			std::string_view password = "";
		};

		virtual ~IClient() noexcept override = default;

		virtual void Connect(const ConnectArgs &address) = 0;
		virtual void Disconnect() = 0;
		virtual bool IsConnecting() const noexcept = 0;
		virtual bool IsConnected() const noexcept = 0;
		virtual void SendReliable(std::string_view data) = 0;
		virtual void SendUnreliable(std::string_view data) = 0;

		TE_FORCEINLINE bool TryDisconnect() noexcept
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

		TE_FORCEINLINE bool IsDisconnected() const noexcept
		{
			return !IsConnected() && !IsConnecting();
		}

	  private:
		void LuaConnect(sol::object args);
		void LuaSendReliable(sol::object data);
		void LuaSendUnreliable(sol::object data);
	};
} // namespace tudov

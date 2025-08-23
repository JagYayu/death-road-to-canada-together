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

#include "NetworkSession.hpp"
#include "util/Definitions.hpp"

#include "sol/forward.hpp"

#include <span>
#include <string_view>

namespace tudov
{
	enum class EClientSessionState : std::uint8_t;
	class LuaAPI;

	struct IClientSession : public INetworkSession
	{
		friend LuaAPI;

		struct ConnectArgs
		{
			virtual ~ConnectArgs() noexcept = default;

			std::string_view password = "";
		};

		virtual ~IClientSession() noexcept override = default;

		virtual ClientSessionToken GetToken() const noexcept = 0;
		virtual EClientSessionState GetSessionState() const noexcept = 0;
		virtual void Connect(const ConnectArgs &address) = 0;
		virtual void Disconnect() = 0;
		virtual bool TryDisconnect() = 0;
		virtual void SendReliable(std::span<const std::byte> data, ChannelID channelID) = 0;
		virtual void SendUnreliable(std::span<const std::byte> data, ChannelID channelID) = 0;

	  private:
		void LuaConnect(sol::object args);
		void LuaSendReliable(sol::object data, sol::object channelID);
		void LuaSendUnreliable(sol::object data, sol::object channelID);
	};
} // namespace tudov

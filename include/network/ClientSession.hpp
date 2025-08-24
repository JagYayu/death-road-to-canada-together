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

#include <string_view>

namespace tudov
{
	enum class EClientSessionState : std::uint8_t;
	struct NetworkSessionData;
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

		virtual ClientSessionID GetSessionID() const noexcept = 0;
		virtual EClientSessionState GetSessionState() const noexcept = 0;
		virtual void Connect(const ConnectArgs &address) = 0;
		virtual void Disconnect() = 0;
		virtual bool TryDisconnect() = 0;
		virtual void SendReliable(const NetworkSessionData &data) = 0;
		virtual void SendUnreliable(const NetworkSessionData &data) = 0;

	  private:
		void LuaConnect(sol::object args);
		void LuaSendReliable(sol::object data, sol::object channelID);
		void LuaSendUnreliable(sol::object data, sol::object channelID);
	};
} // namespace tudov

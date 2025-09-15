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

#include <functional>
#include <string_view>

namespace tudov
{
	enum class EClientSessionState : std::uint8_t;
	enum class EDisconnectionCode : std::uint32_t;
	struct NetworkSessionData;
	class LuaBindings;

	struct ClientSessionConnectArgs
	{
		virtual ~ClientSessionConnectArgs() noexcept = default;

		std::string_view password = "";
	};

	using ClientHostErrorHandler = std::function<void(const ClientSessionConnectArgs &args, std::exception *e)>;

	struct IClientSession : public INetworkSession
	{
		friend LuaBindings;

		using ConnectArgs = ClientSessionConnectArgs;

		virtual ~IClientSession() noexcept override = default;

		virtual ClientSessionID GetSessionID() const noexcept = 0;
		virtual EClientSessionState GetSessionState() const noexcept = 0;
		virtual void Connect(const ConnectArgs &args) = 0;
		virtual void Disconnect(EDisconnectionCode code) = 0;
		virtual bool TryDisconnect(EDisconnectionCode code) = 0;
		virtual void SendReliable(const NetworkSessionData &data) = 0;
		virtual void SendUnreliable(const NetworkSessionData &data) = 0;

		virtual void ConnectAsync(const ConnectArgs &args, const ClientHostErrorHandler &handler) noexcept;

	  private:
		void LuaConnect(sol::object args) noexcept;
		void LuaDisconnect(sol::object code) noexcept;
		void LuaSendReliable(sol::object data, sol::object channelID) noexcept;
		void LuaSendUnreliable(sol::object data, sol::object channelID) noexcept;
	};
} // namespace tudov

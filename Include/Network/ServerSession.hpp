/**
 * @file network/Server.hpp
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
#include "Data/Constants.hpp"
#include "Util/Definitions.hpp"

#include "sol/forward.hpp"

#include <cstdint>
#include <functional>
#include <optional>

namespace tudov
{
	enum class EDisconnectionCode : std::uint32_t;
	enum class EServerSessionState : std::uint8_t;
	struct NetworkSessionData;
	class LuaBindings;

	struct ServerSessionHostArgs
	{
		virtual ~ServerSessionHostArgs() noexcept = default;

		std::string_view title = NetworkServerTitle;
		std::string_view password = NetworkServerPassword;
		std::uint32_t maximumClients = NetworkServerMaximumClients;
	};

	using ServerHostErrorHandler = std::function<void(const ServerSessionHostArgs &args, std::exception *e)>;

	struct IServerSession : public INetworkSession
	{
		friend LuaBindings;

		using HostArgs = ServerSessionHostArgs;

		~IServerSession() noexcept override = default;

		virtual EServerSessionState GetSessionState() const noexcept = 0;
		virtual std::size_t GetClients() const noexcept = 0;
		virtual void Host(const HostArgs &args) = 0;
		virtual void HostAsync(const HostArgs &args, const ServerHostErrorHandler &handler) noexcept = 0;
		virtual void Shutdown() = 0;
		virtual bool TryShutdown() = 0;
		virtual std::optional<std::string_view> GetTitle() const noexcept = 0;
		virtual std::optional<std::string_view> GetPassword() const noexcept = 0;
		virtual std::optional<std::size_t> GetMaxClients() const noexcept = 0;
		virtual void Disconnect(ClientSessionID clientSessionID, EDisconnectionCode code) = 0;
		virtual void SendReliable(ClientSessionID clientSessionID, const NetworkSessionData &data) = 0;
		virtual void SendUnreliable(ClientSessionID clientSessionID, const NetworkSessionData &data) = 0;
		virtual void BroadcastReliable(const NetworkSessionData &data) = 0;
		virtual void BroadcastUnreliable(const NetworkSessionData &data) = 0;

	  private:
		void LuaHost(sol::object args) noexcept;
		void LuaDisconnect(sol::object clientID, sol::object code) noexcept;
		void LuaSendReliable(sol::object clientID, sol::object data, sol::object channelID) noexcept;
		void LuaSendUnreliable(sol::object clientID, sol::object data, sol::object channelID) noexcept;
		void LuaBroadcastReliable(sol::object data, sol::object channelID) noexcept;
		void LuaBroadcastUnreliable(sol::object data, sol::object channelID) noexcept;
	};
} // namespace tudov

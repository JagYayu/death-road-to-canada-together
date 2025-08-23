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
#include "data/Constants.hpp"
#include "util/Definitions.hpp"

#include "sol/forward.hpp"

#include <optional>
#include <span>

namespace tudov
{
	enum class EServerSessionState : std::uint8_t;
	class LuaAPI;

	struct IServerSession : public INetworkSession
	{
		friend LuaAPI;

		struct HostArgs
		{
			virtual ~HostArgs() noexcept = default;

			std::string_view title = NetworkServerTitle;
			std::string_view password = NetworkServerPassword;
			std::uint32_t maximumClients = NetworkServerMaximumClients;
		};

		~IServerSession() noexcept override = default;

		virtual EServerSessionState GetSessionState() const noexcept = 0;
		virtual void Host(const HostArgs &args) = 0;
		virtual void Shutdown() = 0;
		virtual bool TryShutdown() = 0;
		virtual std::optional<std::string_view> GetTitle() const noexcept = 0;
		virtual std::optional<std::string_view> GetPassword() const noexcept = 0;
		virtual std::optional<std::size_t> GetMaxClients() const noexcept = 0;
		virtual void SendReliable(ClientSessionToken clientSessionToken, std::span<const std::byte> data, ChannelID channelID) = 0;
		virtual void SendUnreliable(ClientSessionToken clientSessionToken, std::span<const std::byte> data, ChannelID channelID) = 0;
		virtual void BroadcastReliable(std::span<const std::byte> data, ChannelID channelID) = 0;
		virtual void BroadcastUnreliable(std::span<const std::byte> data, ChannelID channelID) = 0;

	  private:
		void LuaHost(sol::object args) noexcept;
		void LuaSendReliable(sol::object clientID, sol::object data, sol::object channelID) noexcept;
		void LuaSendUnreliable(sol::object clientID, sol::object data, sol::object channelID) noexcept;
		void LuaBroadcastReliable(sol::object data, sol::object channelID) noexcept;
		void LuaBroadcastUnreliable(sol::object data, sol::object channelID) noexcept;
	};
} // namespace tudov

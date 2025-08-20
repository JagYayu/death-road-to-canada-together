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

#include "NetworkComponent.hpp"
#include "data/Constants.hpp"

#include "sol/forward.hpp"

#include <optional>

namespace tudov
{
	class LuaAPI;

	struct IServer : public INetworkComponent
	{
		friend LuaAPI;

		using ClientID = std::uint64_t;

		struct HostArgs
		{
			virtual ~HostArgs() noexcept = default;

			std::string_view title = NetworkServerTitle;
			std::string_view password = NetworkServerPassword;
			std::uint32_t maximumClients = NetworkServerMaximumClients;
		};

		~IServer() noexcept override = default;

		virtual void Host(const HostArgs &args) = 0;
		virtual void Shutdown() = 0;
		virtual bool IsHosting() const noexcept = 0;
		virtual std::optional<std::string_view> GetTitle() const noexcept = 0;
		virtual std::optional<std::string_view> GetPassword() const noexcept = 0;
		virtual std::optional<std::size_t> GetMaxClients() const noexcept = 0;
		virtual void SendReliable(ClientID clientID, std::string_view data) = 0;
		virtual void SendUnreliable(ClientID clientID, std::string_view data) = 0;
		virtual void BroadcastReliable(std::string_view data) = 0;
		virtual void BroadcastUnreliable(std::string_view data) = 0;

		inline bool IsShutdown() noexcept
		{
			return !IsHosting();
		}

	  private:
		void LuaHost(sol::object args) noexcept;
		void LuaSendReliable(sol::object clientID, sol::object data) noexcept;
		void LuaSendUnreliable(sol::object clientID, sol::object data) noexcept;
		void LuaBroadcastReliable(sol::object data) noexcept;
		void LuaBroadcastUnreliable(sol::object data) noexcept;
	};
} // namespace tudov

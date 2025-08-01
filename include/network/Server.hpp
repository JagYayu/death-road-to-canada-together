#pragma once

#include "NetworkComponent.hpp"
#include "data/Constants.hpp"

#include <optional>

namespace tudov
{
	struct IServer : public INetworkComponent
	{
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
		virtual bool IsHosting() noexcept = 0;
		virtual std::optional<std::string_view> GetTitle() noexcept = 0;
		virtual std::optional<std::string_view> GetPassword() noexcept = 0;
		virtual std::optional<std::size_t> GetMaxClients() noexcept = 0;
		virtual void SendReliable(ClientID clientID, std::string_view data) = 0;
		virtual void SendUnreliable(ClientID clientID, std::string_view data) = 0;
		virtual void BroadcastReliable(std::string_view data) = 0;
		virtual void BroadcastUnreliable(std::string_view data) = 0;

		inline bool IsShutdown() noexcept
		{
			return !IsHosting();
		}
	};
} // namespace tudov

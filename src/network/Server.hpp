#pragma once

#include "NetworkComponent.hpp"
#include "util/Defs.hpp"

#include <optional>
#include <string_view>

namespace tudov
{
	struct IServer : public INetworkComponent
	{
		struct HostArgs
		{
			virtual ~HostArgs() noexcept = default;

			std::string_view title = NetworkServerTitle;
			std::string_view password = NetworkServerPassword;
			std::uint32_t maximumClients = NetworkServerMaximumClients;
		};

		virtual ~IServer() noexcept override = default;

		virtual void Host(const HostArgs &args) = 0;
		virtual void Shutdown() = 0;
		virtual bool IsHosting() noexcept = 0;
		virtual std::optional<std::string_view> GetTitle() noexcept = 0;
		virtual std::optional<std::string_view> GetPassword() noexcept = 0;
		virtual std::optional<std::int32_t> GetMaxClients() noexcept = 0;

		inline bool IsShutdown() noexcept
		{
			return !IsHosting();
		}
	};
} // namespace tudov

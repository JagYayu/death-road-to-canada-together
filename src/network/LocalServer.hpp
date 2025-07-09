#pragma once

#include "Server.hpp"
#include "SocketType.hpp"

#include <memory>
#include <string_view>

namespace tudov
{
	class LocalServer : public IServer
	{
	  public:
		struct LocalHostArgs : public HostArgs
		{
			std::string_view host;
			std::string_view port;
		};

	  private:
		struct HostInfo
		{
			std::string_view title = NetworkServerTitle;
			std::string_view password = NetworkServerPassword;
			std::uint32_t maximumClients = NetworkServerMaximumClients;
		};

	  private:
		INetwork &_network;
		ESocketType _socketType;
		std::unique_ptr<HostInfo> _hostInfo;

	  public:
		explicit LocalServer(INetwork &network) noexcept;
		~LocalServer() noexcept = default;

	  private:
		void TryCreateENetHost();

	  public:
		INetwork &GetNetwork() noexcept override;
		ESocketType GetSocketType() const noexcept override;
		std::optional<std::string_view> GetTitle() noexcept override;
		std::optional<std::string_view> GetPassword() noexcept override;
		std::optional<std::int32_t> GetMaxClients() noexcept override;

		bool IsHosting() noexcept override;
		void Host(const HostArgs &args) override;
		void Shutdown() override;

		bool Update() override;

		void Host(const LocalHostArgs &args)
		{
			return Host(static_cast<HostArgs>(args));
		}
	};
} // namespace tudov

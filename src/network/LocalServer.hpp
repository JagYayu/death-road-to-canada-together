#pragma once

#include "Server.hpp"
#include "SocketType.hpp"

#include <memory>
#include <string_view>
#include <unordered_map>

namespace tudov
{
	class LocalClient;

	class LocalServer : public IServer
	{
	  public:
		using HostArgs = IServer::HostArgs;

	  private:
		struct HostInfo
		{
			std::string_view title = NetworkServerTitle;
			std::string_view password = NetworkServerPassword;
			std::uint32_t maximumClients = NetworkServerMaximumClients;
			std::unordered_map<std::uint32_t, std::weak_ptr<LocalClient>> localClients;
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

		void AddClient(std::uint32_t uid, const std::weak_ptr<LocalClient> &localClient);
	};
} // namespace tudov

#pragma once

#include "Network.hpp"
#include "Server.hpp"
#include "debug/Debug.hpp"

struct _ENetHost;
struct _ENetPeer;

namespace tudov
{
	class ReliableUDPServer : public IServer, public IDebugProvider
	{
	  private:
		INetwork &_network;
		_ENetHost *_eNetHost;
		_ENetPeer *_eNetPeer;

	  public:
		explicit ReliableUDPServer(INetwork &network) noexcept;
		~ReliableUDPServer() noexcept override;

		ESocketType GetSocketType() const noexcept override;
		INetwork &GetNetwork() noexcept override;
		bool Update() noexcept override;

		void ProvideDebug(IDebugManager &debugManager) noexcept override;

		void Host(const HostArgs &args) override;
		void Shutdown() override;
		bool IsHosting() noexcept override;
		std::optional<std::string_view> GetTitle() noexcept override;
		std::optional<std::string_view> GetPassword() noexcept override;
		std::optional<std::int32_t> GetMaxClients() noexcept override;
	};
} // namespace tudov

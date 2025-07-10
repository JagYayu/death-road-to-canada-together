#pragma once

#include "Client.hpp"
#include "Server.hpp"
#include "SocketType.hpp"
#include "program/EngineComponent.hpp"
#include "util/Log.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace tudov
{
	struct INetwork : public IEngineComponent, public ILogProvider
	{
		~INetwork() noexcept override = default;

		virtual IClient *GetClient(std::int32_t uid = 0) noexcept = 0;
		virtual IServer *GetServer(std::int32_t uid = 0) noexcept = 0;
		virtual std::vector<std::weak_ptr<IClient>> GetClients() noexcept = 0;
		virtual std::vector<std::weak_ptr<IServer>> GetServers() noexcept = 0;
		virtual void ChangeClientSocket(ESocketType socketType, std::int32_t uid = 0) = 0;
		virtual void ChangeServerSocket(ESocketType socketType, std::int32_t uid = 0) = 0;

		virtual std::int32_t GetLimitsPerUpdate() noexcept;

		virtual bool Update() noexcept;

		inline const IClient *GetClient() const noexcept
		{
			return const_cast<INetwork *>(this)->GetClient();
		}

		inline const IServer *GetServer() const noexcept
		{
			return const_cast<INetwork *>(this)->GetServer();
		}
	};

	class Network : public INetwork
	{
	  private:
		std::shared_ptr<Log> _log;
		Context &_context;
		std::unordered_map<std::int32_t, std::shared_ptr<IClient>> _clients;
		std::unordered_map<std::int32_t, std::shared_ptr<IServer>> _servers;
		ESocketType _socketType;

	  public:
		explicit Network(Context &context, ESocketType socketType = ESocketType::Local) noexcept;
		~Network() noexcept override = default;

		Context &GetContext() noexcept override;
		void Initialize() noexcept override;
		void Deinitialize() noexcept override;

		IClient *GetClient(std::int32_t uid = 0) noexcept override;
		IServer *GetServer(std::int32_t uid = 0) noexcept override;
		std::vector<std::weak_ptr<IClient>> GetClients() noexcept override;
		std::vector<std::weak_ptr<IServer>> GetServers() noexcept override;
		void ChangeClientSocket(ESocketType socketType, std::int32_t uid = 0) override;
		bool Update() noexcept override;
	};
} // namespace tudov

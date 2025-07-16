#pragma once

#include "Client.hpp"
#include "Server.hpp"
#include "SocketType.hpp"
#include "debug/Debug.hpp"
#include "program/EngineComponent.hpp"
#include "util/Log.hpp"

#include <memory>
#include <optional>

namespace tudov
{
	struct INetwork : public IEngineComponent, public ILogProvider
	{
		static constexpr std::int32_t DefaultUID = 0;

		~INetwork() noexcept override = default;

		virtual IClient *GetClient(std::int32_t uid = 0) noexcept = 0;
		virtual IServer *GetServer(std::int32_t uid = 0) noexcept = 0;
		virtual std::vector<std::weak_ptr<IClient>> GetClients() noexcept = 0;
		virtual std::vector<std::weak_ptr<IServer>> GetServers() noexcept = 0;
		virtual void SetClient(ESocketType socketType, std::int32_t uid = DefaultUID) = 0;
		virtual void SetServer(ESocketType socketType, std::int32_t uid = DefaultUID) = 0;

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

	class Network : public INetwork, public IDebugProvider
	{
	  private:
		std::shared_ptr<Log> _log;
		Context &_context;
		std::unordered_map<std::int32_t, std::shared_ptr<IClient>> _clients;
		std::unordered_map<std::int32_t, std::shared_ptr<IServer>> _servers;
		bool _initialized;
		ESocketType _socketType;
		std::int32_t _debugClientUID;
		std::int32_t _debugServerUID;

	  public:
		explicit Network(Context &context) noexcept;
		~Network() noexcept override = default;

		Log &GetLog() noexcept override;

		void ProvideDebug(IDebugManager &debugManager) noexcept override;

		Context &GetContext() noexcept override;
		void Initialize() noexcept override;
		void Deinitialize() noexcept override;

		IClient *GetClient(std::int32_t uid = DefaultUID) noexcept override;
		IServer *GetServer(std::int32_t uid = DefaultUID) noexcept override;
		std::vector<std::weak_ptr<IClient>> GetClients() noexcept override;
		std::vector<std::weak_ptr<IServer>> GetServers() noexcept override;
		void SetClient(ESocketType socketType, std::int32_t uid = DefaultUID) override;
		void SetServer(ESocketType socketType, std::int32_t uid = DefaultUID) override;
		bool Update() noexcept override;
	};
} // namespace tudov

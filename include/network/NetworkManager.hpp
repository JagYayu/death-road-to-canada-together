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
	struct INetworkManager : public IEngineComponent, public ILogProvider
	{
		static constexpr std::int32_t DefaultUID = 0;

		~INetworkManager() noexcept override = default;

		virtual IClient *GetClient(std::int32_t uid = 0) noexcept = 0;
		virtual IServer *GetServer(std::int32_t uid = 0) noexcept = 0;
		virtual std::vector<std::weak_ptr<IClient>> GetClients() noexcept = 0;
		virtual std::vector<std::weak_ptr<IServer>> GetServers() noexcept = 0;
		virtual bool SetClient(std::int32_t uid = DefaultUID) = 0;
		/**
		 * @brief Change client socket for specific uid. Do nothing and return false if socket type duplicated.
		 *
		 * @param[in]  socketType @see ::tudov::ESocketType
		 * @param[in]  uid @default ::tudov::INetworkManager::DefaultUID
		 *
		 * @return successful
		 */
		virtual bool SetClient(ESocketType socketType, std::int32_t uid = DefaultUID) = 0;
		virtual bool SetServer(std::int32_t uid = DefaultUID) = 0;
		/**
		 * @brief Change server socket for specific uid. Do nothing and return false if socket type duplicated.
		 *
		 * @param[in]  socketType @see tudov::ESocketType
		 * @param[in]  uid @default DefaultUID
		 *
		 * @return successful
		 */
		virtual bool SetServer(ESocketType socketType, std::int32_t uid = DefaultUID) = 0;

		virtual std::int32_t GetLimitsPerUpdate() noexcept;

		virtual bool Update() noexcept;

		inline const IClient *GetClient() const noexcept
		{
			return const_cast<INetworkManager *>(this)->GetClient();
		}

		inline const IServer *GetServer() const noexcept
		{
			return const_cast<INetworkManager *>(this)->GetServer();
		}
	};

	class NetworkManager : public INetworkManager, public IDebugProvider
	{
	  protected:
		std::shared_ptr<Log> _log;
		Context &_context;
		std::unordered_map<std::int32_t, std::shared_ptr<IClient>> _clients;
		std::unordered_map<std::int32_t, std::shared_ptr<IServer>> _servers;
		bool _initialized;
		ESocketType _socketType;
		std::int32_t _debugClientUID;
		std::int32_t _debugServerUID;

	  public:
		explicit NetworkManager(Context &context) noexcept;
		~NetworkManager() noexcept override = default;

	  protected:
		std::vector<DebugConsoleResult> DebugClientConnect(std::string_view arg) noexcept;
		std::vector<DebugConsoleResult> DebugServerHost(std::string_view arg) noexcept;

	  public:
		Log &GetLog() noexcept override;

		void ProvideDebug(IDebugManager &debugManager) noexcept override;

		Context &GetContext() noexcept override;
		void Initialize() noexcept override;
		void Deinitialize() noexcept override;

		IClient *GetClient(std::int32_t uid = DefaultUID) noexcept override;
		IServer *GetServer(std::int32_t uid = DefaultUID) noexcept override;
		std::vector<std::weak_ptr<IClient>> GetClients() noexcept override;
		std::vector<std::weak_ptr<IServer>> GetServers() noexcept override;
		bool SetClient(std::int32_t uid = DefaultUID) override;
		bool SetClient(ESocketType socketType, std::int32_t uid = DefaultUID) override;
		bool SetServer(std::int32_t uid = DefaultUID) override;
		bool SetServer(ESocketType socketType, std::int32_t uid = DefaultUID) override;
		bool Update() noexcept override;
	};
} // namespace tudov

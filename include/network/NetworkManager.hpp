/**
 * @file network/NetworkManager.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "debug/Debug.hpp"
#include "program/EngineComponent.hpp"
#include "system/Log.hpp"
#include "util/Definitions.hpp"

#include "sol/forward.hpp"

#include <memory>

namespace tudov
{
	enum class ESocketType : std::uint8_t;
	struct IClientSession;
	struct IServerSession;

	struct INetworkManager : public IEngineComponent, private ILogProvider
	{
		static constexpr std::int32_t DefaultSessionSlot = 0;

		~INetworkManager() noexcept override = default;

		/**
		 * Get client with specific UID.
		 */
		virtual IClientSession *GetClient(NetworkSessionSlot uid = 0) noexcept = 0;

		/**
		 * Get server with specific UID.
		 */
		virtual IServerSession *GetServer(NetworkSessionSlot uid = 0) noexcept = 0;

		virtual std::vector<std::weak_ptr<IClientSession>> GetClients() noexcept = 0;

		virtual std::vector<std::weak_ptr<IServerSession>> GetServers() noexcept = 0;

		/**
		 * Remove client.
		 */
		virtual bool SetClient(NetworkSessionSlot clientSlot = DefaultSessionSlot) = 0;

		/**
		 * @brief Change client socket for specific uid. Do nothing and return false if socket type duplicated.
		 *
		 * @param[in]  socketType @see ::tudov::ESocketType
		 * @param[in]  uid @default ::tudov::INetworkManager::DefaultUID
		 *
		 * @return successful
		 */
		virtual bool SetClient(ESocketType socketType, NetworkSessionSlot clientSlot = DefaultSessionSlot) = 0;

		/**
		 * Remove server.
		 */
		virtual bool SetServer(NetworkSessionSlot serverSlot = DefaultSessionSlot) = 0;

		/**
		 * @brief Change server socket for specific uid. Do nothing and return false if socket type duplicated.
		 *
		 * @param[in]  socketType @see tudov::ESocketType
		 * @param[in]  uid @default DefaultUID
		 *
		 * @return successful
		 */
		virtual bool SetServer(ESocketType socketType, NetworkSessionSlot serverSlot = DefaultSessionSlot) = 0;

		/**
		 * Get the loop limits when doing update.
		 */
		virtual std::int32_t GetLimitsPerUpdate() noexcept;

		/**
		 * Update clients and servers.
		 */
		virtual bool Update() noexcept;

		inline const IClientSession *GetClient() const noexcept
		{
			return const_cast<INetworkManager *>(this)->GetClient();
		}

		inline const IServerSession *GetServer() const noexcept
		{
			return const_cast<INetworkManager *>(this)->GetServer();
		}
	};

	class LuaAPI;

	class NetworkManager : public INetworkManager, public IDebugProvider
	{
		friend LuaAPI;

	  protected:
		std::shared_ptr<Log> _log;
		Context &_context;
		std::unordered_map<std::int32_t, std::shared_ptr<IClientSession>> _clients;
		std::unordered_map<std::int32_t, std::shared_ptr<IServerSession>> _servers;
		bool _initialized;
		ESocketType _socketType;
		NetworkSessionSlot _debugClientSlot;
		NetworkSessionSlot _debugServerSlot;

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

		IClientSession *GetClient(NetworkSessionSlot clientSlot = DefaultSessionSlot) noexcept override;
		IServerSession *GetServer(NetworkSessionSlot serverSlot = DefaultSessionSlot) noexcept override;
		std::vector<std::weak_ptr<IClientSession>> GetClients() noexcept override;
		std::vector<std::weak_ptr<IServerSession>> GetServers() noexcept override;
		bool SetClient(NetworkSessionSlot clientSlot = DefaultSessionSlot) override;
		bool SetClient(ESocketType socketType, NetworkSessionSlot clientSlot = DefaultSessionSlot) override;
		bool SetServer(NetworkSessionSlot serverSlot = DefaultSessionSlot) override;
		bool SetServer(ESocketType socketType, NetworkSessionSlot serverSlot = DefaultSessionSlot) override;
		bool Update() noexcept override;

	  private:
		IClientSession *LuaGetClient(sol::object uid) noexcept;
		IServerSession *LuaGetServer(sol::object uid) noexcept;
	};
} // namespace tudov

#pragma once

#include "Client.hpp"
#include "Server.hpp"
#include "SocketType.hpp"
#include "program/EngineComponent.hpp"
#include "util/Log.hpp"
#include <memory>

namespace tudov
{
	struct INetwork : public IEngineComponent
	{
		virtual IClient &GetClient() noexcept = 0;
		virtual IServer &GetServer() noexcept = 0;
		virtual void ChangeSocket(ESocketType socketType) = 0;

		virtual inline std::int32_t GetLimitsPerUpdate() noexcept
		{
			return 3;
		}

		virtual inline bool Update() noexcept
		{
			auto &&client = GetClient();
			auto &&server = GetServer();

			std::int32_t limit = GetLimitsPerUpdate();
			if (limit <= 0)
			{
				return true;
			}

			while (client.Update() | server.Update())
			{
				if (limit <= 0)
				{
					return true;
				}
				--limit;
			}

			return false;
		}

		inline const IClient &GetClient() const noexcept
		{
			return const_cast<INetwork *>(this)->GetClient();
		}

		inline const IServer &GetServer() const noexcept
		{
			return const_cast<INetwork *>(this)->GetServer();
		}
	};

	class Network : public INetwork
	{
	  private:
		std::shared_ptr<Log> _log;
		Context &_context;
		std::shared_ptr<IClient> _client;
		std::shared_ptr<IServer> _server;
		ESocketType _socketType;

	  public:
		explicit Network(Context &context, ESocketType socketType = ESocketType::Local) noexcept;

		Context &GetContext() noexcept override;
		void Initialize() noexcept override;
		void Deinitialize() noexcept override;

		IClient &GetClient() noexcept override;
		IServer &GetServer() noexcept override;
		void ChangeSocket(ESocketType socketType) override;
		bool Update() noexcept override;
	};
} // namespace tudov

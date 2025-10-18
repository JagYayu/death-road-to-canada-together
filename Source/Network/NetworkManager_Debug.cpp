/**
 * @file network/NetworkManager_Debug.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Debug/DebugConsole.hpp"

#include "Debug/DebugConsole.hpp"
#include "Debug/DebugManager.hpp"
#include "Exception/Exception.hpp"
#include "Network/LocalClientSession.hpp"
#include "Network/LocalServerSession.hpp"
#include "Network/NetworkSessionData.hpp"
#include "Network/ReliableUDPClientSession.hpp"
#include "Network/ReliableUDPServerSession.hpp"
#include "Network/SocketType.hpp"
#include "Util/Definitions.hpp"

#include <exception>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

using namespace tudov;

void NetworkManager::ProvideDebug(IDebugManager &debugManager) noexcept
{
	if (DebugConsole *console = debugManager.GetElement<DebugConsole>(); console != nullptr)
	{
		auto &&clientConnect = [this](std::string_view arg)
		{
			return DebugClientConnect(arg);
		};

		console->SetCommand(DebugConsole::Command{
		    .name = "clientConnect",
		    .help = "clientConnect <socket> <address>: Connect current client to a server.",
		    .func = clientConnect,
		});

		auto &&clientInfo = [this](std::string_view arg)
		{
			std::vector<DebugConsole::Result> results{};

			return results;
		};

		console->SetCommand(DebugConsole::Command{
		    .name = "clientInfo",
		    .help = "clientInfo [uid]: Check client's connection info.",
		    .func = clientInfo,
		});

		auto &&clientSet = [this](std::string_view arg)
		{
			NetworkSessionSlot prevUID = _debugClientSlot;
			_debugClientSlot = std::stoi(arg.data());

			std::vector<DebugConsole::Result> results{};
			results.emplace_back(DebugConsole::Result{
			    .message = std::format("Changed client uid from {} to {}", prevUID, _debugClientSlot),
			    .code = DebugConsole::Code::Success,
			});
			return results;
		};

		console->SetCommand(DebugConsole::Command{
		    .name = "clientSet",
		    .help = "clientSet <uid>: Change current client to specific uid.",
		    .func = clientSet,
		});

		auto &&clientSend = [this](std::string_view arg)
		{
			std::vector<DebugConsole::Result> results;

			try
			{
				NetworkSessionData data{
				    .bytes = std::span<const std::byte>(reinterpret_cast<const std::byte *>(arg.data()), arg.size()),
				    .channelID = 0,
				};

				const std::shared_ptr<IClientSession> &client = _clients.at(_debugClientSlot);

				client->SendReliable(data);

				auto &&msg = std::format("Client<{}>{} sent {} message to server: {}",
				                         client->GetSessionSlot(), client->GetSessionID(), "reliable", arg);
				results.emplace_back(msg, DebugConsole::Code::Success);
			}
			catch (const std::exception &e)
			{
				results.emplace_back(DebugConsole::Result{
				    .message = std::format("{}", e.what()),
				    .code = DebugConsole::Code::Failure,
				});
			}

			return results;
		};

		console->SetCommand(DebugConsole::Command{
		    .name = "clientSend",
		    .help = "clientSend <message> [unreliable]: Send reliable/unreliable message to server",
		    .func = clientSend,
		});

		auto &&serverHost = [this](std::string_view arg)
		{
			return DebugServerHost(arg);
		};

		console->SetCommand(DebugConsole::Command{
		    .name = "serverHost",
		    .help = "serverHost <socket> <address>: Host server.",
		    .func = serverHost,
		});

		auto &&serverInfo = [this](std::string_view arg)
		{
			std::vector<DebugConsole::Result> results{};
			return results;
		};

		console->SetCommand(DebugConsole::Command{
		    .name = "serverInfo",
		    .help = "serverInfo [uid]: Check server's hosting info.",
		    .func = serverInfo,
		});
	}
}

std::vector<DebugConsoleResult> NetworkManager::DebugClientConnect(std::string_view arg) noexcept
{
	std::vector<DebugConsole::Result> results{};

	std::string_view address;
	{
		auto spacePos = arg.find(' ');
		if (spacePos == std::string_view::npos)
		{
			results.emplace_back("Bad command", DebugConsole::Code::Failure);
			return results;
		}

		std::string_view socket = arg.substr(0, spacePos);
		auto optSocketType = StringToSocketType(socket);
		if (!optSocketType.has_value())
		{
			results.emplace_back("Socket not provided or invalid", DebugConsole::Code::Failure);
			return results;
		}

		try
		{
			SetClient(optSocketType.value(), _debugClientSlot);
		}
		catch (const Exception &e)
		{
			results.emplace_back(std::format("Failed to set client socket: {}", e.what()), DebugConsole::Code::Failure);
			return results;
		}

		address = arg.substr(spacePos + 1);
		address.remove_prefix(std::min(address.find_first_not_of(" "), address.size()));
	}

	IClientSession *client = GetClient(_debugClientSlot);
	try
	{
		if (auto *localClient = dynamic_cast<LocalClientSession *>(client); localClient != nullptr)
		{
			LocalServerSession *server = nullptr;
			try
			{
				server = dynamic_cast<LocalServerSession *>(GetServer(std::stoi(address.data())));
			}
			catch (std::exception &)
			{
			}

			LocalClientSession::ConnectArgs args;
			args.localServer = server;

			localClient->Connect(args);
			results.emplace_back("Connecting to local server", DebugConsole::Code::Success);
		}
		else if (auto *reliableUDPClient = dynamic_cast<ReliableUDPClientSession *>(client); reliableUDPClient != nullptr)
		{

			std::istringstream iss{std::string(address)};
			std::string host;
			std::string port;
			iss >> host >> port;

			ReliableUDPClientSession::ConnectArgs args;
			args.host = host;
			args.port = std::stoi(port);

			reliableUDPClient->Connect(args);

			results.emplace_back(std::format("Connecting to reliable udp server {} {}", host, port), DebugConsole::Code::Success);
		}
		else
		{
			results.emplace_back("Unhandled client connecting logic", DebugConsole::Code::Failure);
		}
	}
	catch (const std::exception &e)
	{
		results.emplace_back(e.what(), DebugConsole::Code::Failure);
	}

	return results;
}

std::vector<DebugConsoleResult> NetworkManager::DebugServerHost(std::string_view arg) noexcept
{
	std::vector<DebugConsole::Result> results{};

	std::string_view address;
	{
		auto spacePos = arg.find(' ');
		if (spacePos == std::string_view::npos)
		{
			results.emplace_back("Bad command", DebugConsole::Code::Failure);
			return results;
		}

		auto socket = arg.substr(0, spacePos);
		auto &&socketType = StringToSocketType(socket);
		if (!socketType.has_value())
		{
			results.emplace_back("Socket not provided or invalid", DebugConsole::Code::Failure);
			return results;
		}

		try
		{
			SetServer(socketType.value(), _debugServerSlot);
		}
		catch (const Exception &e)
		{
			results.emplace_back(e.what(), DebugConsole::Code::Failure);
			return results;
		}

		address = arg.substr(spacePos + 1);
		address.remove_prefix(std::min(address.find_first_not_of(" "), address.size()));
	}

	IServerSession *server = GetServer(_debugServerSlot);
	try
	{
		if (auto &&localServer = dynamic_cast<LocalServerSession *>(server); localServer != nullptr)
		{
			LocalServerSession::HostArgs args;

			localServer->Host(args);

			results.emplace_back("Hosting local server", DebugConsole::Code::Success);
		}
		else if (auto &&reliableUDPServer = dynamic_cast<ReliableUDPServerSession *>(server); reliableUDPServer != nullptr)
		{
			std::istringstream iss{std::string(address)};
			std::string host;
			std::string port;
			iss >> host >> port;

			ReliableUDPServerSession::HostArgs args;
			args.host = host;
			args.port = std::stoi(port);
			reliableUDPServer->Host(args);

			results.emplace_back(std::format("Hosting reliable udp server {} {}", host, port), DebugConsole::Code::Success);
		}
		else
		{
			results.emplace_back("Unhandled server hosting logic", DebugConsole::Code::Failure);
		}
	}
	catch (const std::exception &e)
	{
		results.emplace_back(e.what(), DebugConsole::Code::Failure);
	}

	return results;
}

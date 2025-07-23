#include "network/SocketType.hpp"

using namespace tudov;

std::optional<ESocketType> tudov::StringToSocketType(std::string_view str) noexcept
{
	if (str == "Local")
		return std::make_optional(ESocketType::Local);
	if (str == "RUDP")
		return std::make_optional(ESocketType::RUDP);
	if (str == "Steam")
		return std::make_optional(ESocketType::Steam);
	return std::nullopt;
}

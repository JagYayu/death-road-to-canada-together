#include "mod/ModIO.hpp"

using namespace tudov;

ModIO::ModIO() noexcept
{
}

constexpr EModPortal ModIO::GetModPortalType() const noexcept
{
	return EModPortal::ModIO;
}

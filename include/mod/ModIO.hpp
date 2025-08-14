#include "ModPortal.hpp"

namespace tudov
{
	class ModIO : public IModPortal
	{
	  public:
		explicit ModIO() noexcept;

		constexpr EModPortal GetModPortalType() const noexcept override;
	};
} // namespace tudov

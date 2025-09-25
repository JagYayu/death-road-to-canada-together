/**
 * @file mod/ModIO.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "ModPortal.hpp"

namespace tudov
{
	class ModIO final : public IModPortal
	{
	  public:
		explicit ModIO() noexcept;

		constexpr EModPortal GetModPortalType() const noexcept override;
	};
} // namespace tudov

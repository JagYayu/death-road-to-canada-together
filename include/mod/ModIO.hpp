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
		explicit ModIO(const ModIO &) noexcept = default;
		explicit ModIO(ModIO &&) noexcept = default;
		ModIO &operator=(const ModIO &) noexcept = default;
		ModIO &operator=(ModIO &&) noexcept = default;
		~ModIO() noexcept = default;

		constexpr EModPortal GetModPortalType() const noexcept override;
	};
} // namespace tudov

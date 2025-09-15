/**
 * @file program/Mouse.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "util/Definitions.hpp"

namespace tudov
{
	struct IMouse
	{
		virtual MouseID GetMouseID() noexcept = 0;
	};

	class Mouse : public IMouse
	{
	  private:
		MouseID _mouseID;

	  public:
		explicit Mouse(MouseID mouseID) noexcept;

		MouseID GetMouseID() noexcept;
	};
} // namespace tudov

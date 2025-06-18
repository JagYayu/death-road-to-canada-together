#pragma once

#include <string>
#include <vector>

namespace tudov
{
	class ErrorOverlay
	{
	  public:
		void RenderLoadtimeErrors(std::vector<std::string> errorMessages) noexcept;
	};
} // namespace tudov

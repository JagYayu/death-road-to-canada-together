#pragma once

#include "DebugElement.hpp"

namespace tudov
{
	class DebugLog : public IDebugElement
	{
	  public:
		inline static constexpr std::string_view Name() noexcept
		{
			return "Log";
		}

	  public:
		explicit DebugLog() noexcept;
		~DebugLog() noexcept override = default;

		std::string_view GetName() noexcept override;
		void UpdateAndRender(IWindow &window) noexcept override;
	};
} // namespace tudov

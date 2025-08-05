#pragma once

#include "DebugElement.hpp"

#include <memory>

namespace tudov
{
	class IWindow;

	class DebugScripts : public IDebugElement
	{
	  public:
		inline static constexpr std::string_view Name() noexcept
		{
			return "Scripts";
		}

	  public:
		std::weak_ptr<IWindow> window;

		explicit DebugScripts() noexcept = default;
		~DebugScripts() noexcept = default;

		std::string_view GetName() noexcept override;
		void UpdateAndRender(IWindow &window) noexcept override;
	};
} // namespace tudov
#pragma once

#include "Window.hpp"

#include <memory>
#include <string_view>

namespace tudov
{
	struct EventHandleKey;

	class UtilityWindow : public Window
	{
	  private:
		std::unique_ptr<EventHandleKey> _key;

	  public:
		explicit UtilityWindow(Context &context, std::string_view logName, std::string_view keyName) noexcept;
		~UtilityWindow() noexcept override;

		EventHandleKey GetKey() const noexcept override;
		
		void InitializeWindow(std::int32_t width, std::int32_t height, std::string_view title) noexcept override;
		void DeinitializeWindow() noexcept override;
	};
} // namespace tudov

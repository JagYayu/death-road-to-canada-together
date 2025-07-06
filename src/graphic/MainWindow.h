#pragma once

#include "Window.h"

namespace tudov
{
	class MainWindow : public Window
	{
	  public:
		explicit MainWindow(Engine &engine) noexcept;

		virtual void Initialize(std::int32_t width, std::int32_t height, std::string_view title) noexcept override;
		virtual EventHandleKey GetKey() const noexcept override;
		virtual void HandleEvents() noexcept override;
		virtual void Render() noexcept override;
	};
} // namespace tudov

#pragma once

#include "Window.hpp"

namespace tudov
{
	class MainWindow : public Window
	{
	  public:
		explicit MainWindow(Context &context) noexcept;
		~MainWindow() noexcept override;

		virtual void Initialize(std::int32_t width, std::int32_t height, std::string_view title) noexcept override;
		virtual EventHandleKey GetKey() const noexcept override;
		virtual bool HandleEvent(SDL_Event &event) noexcept override;
		virtual void Render() noexcept override;
	};
} // namespace tudov

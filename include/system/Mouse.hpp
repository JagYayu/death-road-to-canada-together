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

#include "program/Context.hpp"
#include "util/Definitions.hpp"

#include <cmath>
#include <memory>
#include <string_view>
#include <tuple>

namespace tudov
{
	enum class EMouseButton : std::int8_t;
	enum class EMouseButtonFlag : std::int8_t;
	struct IWindow;
	class Context;
	class LuaBindings;
	class Window;

	struct IMouse
	{
		virtual MouseID GetMouseID() const noexcept = 0;

		virtual std::string_view GetMouseName() const noexcept = 0;

		virtual std::shared_ptr<IWindow> GetFocusedWindow() noexcept = 0;

		virtual std::tuple<std::float_t, std::float_t> GetMousePosition() const noexcept = 0;

		virtual bool IsMouseButtonDown(EMouseButton button) const noexcept = 0;

		virtual bool IsMouseButtonsDown(EMouseButtonFlag buttonFlags) const noexcept = 0;

		virtual std::tuple<std::float_t, std::float_t, EMouseButtonFlag> GetMouseState() const noexcept = 0;
	};

	class Mouse : public IMouse
	{
		friend LuaBindings;

	  private:
		MouseID _mouseID;

	  public:
		explicit Mouse(MouseID mouseID) noexcept;

		MouseID GetMouseID() const noexcept override;
		std::string_view GetMouseName() const noexcept override;
		std::shared_ptr<IWindow> GetFocusedWindow() noexcept override;
		std::tuple<std::float_t, std::float_t> GetMousePosition() const noexcept override;
		bool IsMouseButtonDown(EMouseButton button) const noexcept override;
		bool IsMouseButtonsDown(EMouseButtonFlag buttonFlags) const noexcept override;
		std::tuple<std::float_t, std::float_t, EMouseButtonFlag> GetMouseState() const noexcept override;

	  private:
		std::shared_ptr<Window> LuaGetFocusedWindow() noexcept;
	};

	class PrimaryMouse : public Mouse, public IContextProvider
	{
	  private:
		Context &_context;

	  public:
		PrimaryMouse(Context &context) noexcept;

		Context &GetContext() noexcept override;
		std::shared_ptr<IWindow> GetFocusedWindow() noexcept override;
		std::tuple<std::float_t, std::float_t> GetMousePosition() const noexcept override;
		bool IsMouseButtonDown(EMouseButton button) const noexcept override;
		bool IsMouseButtonsDown(EMouseButtonFlag buttonFlags) const noexcept override;
		std::tuple<std::float_t, std::float_t, EMouseButtonFlag> GetMouseState() const noexcept override;
	};
} // namespace tudov

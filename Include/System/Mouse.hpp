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

#include "Log.hpp"
#include "Program/Context.hpp"
#include "Util/Definitions.hpp"

#include <cmath>
#include <memory>
#include <string_view>
#include <tuple>

namespace tudov
{
	enum class EMouseButton : std::int8_t;
	enum class EMouseButtonFlag : std::int8_t;
	struct IWindow;
	class AppEvent;
	class Context;
	class LuaBindings;
	class Window;

	struct IMouse
	{
		virtual MouseID GetMouseID() const noexcept = 0;

		virtual bool HandleEvent(AppEvent &appEvent) noexcept = 0;

		virtual std::string_view GetMouseName() const noexcept = 0;

		virtual std::shared_ptr<IWindow> GetFocusedWindow() noexcept = 0;

		virtual std::tuple<std::float_t, std::float_t> GetMousePosition() const noexcept = 0;

		virtual bool IsMouseButtonDown(EMouseButton button) const noexcept = 0;

		virtual bool IsMouseButtonsDown(EMouseButtonFlag buttonFlags) const noexcept = 0;

		virtual std::tuple<std::float_t, std::float_t, EMouseButtonFlag> GetMouseState() const noexcept = 0;
	};

	class Mouse : public IMouse, public IContextProvider, public std::enable_shared_from_this<Mouse>, private ILogProvider
	{
		friend LuaBindings;

	  private:
		Context &_context;
		MouseID _mouseID;

	  public:
		explicit Mouse(Context &_context, MouseID mouseID) noexcept;
		explicit Mouse(const Mouse &) noexcept = default;
		explicit Mouse(Mouse &&) noexcept = default;
		Mouse &operator=(const Mouse &) noexcept = delete;
		Mouse &operator=(Mouse &&) noexcept = delete;
		~Mouse() noexcept = default;

		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;
		MouseID GetMouseID() const noexcept override;
		std::string_view GetMouseName() const noexcept override;
		std::shared_ptr<IWindow> GetFocusedWindow() noexcept override;
		std::tuple<std::float_t, std::float_t> GetMousePosition() const noexcept override;
		bool IsMouseButtonDown(EMouseButton button) const noexcept override;
		bool IsMouseButtonsDown(EMouseButtonFlag buttonFlags) const noexcept override;
		std::tuple<std::float_t, std::float_t, EMouseButtonFlag> GetMouseState() const noexcept override;
		bool HandleEvent(AppEvent &appEvent) noexcept override;

	  private:
		std::shared_ptr<Window> LuaGetFocusedWindow() noexcept;
	};

	class PrimaryMouse final : public Mouse
	{
	  public:
		PrimaryMouse(Context &context) noexcept;

		Log &GetLog() noexcept override;

		std::shared_ptr<IWindow> GetFocusedWindow() noexcept override;
		std::tuple<std::float_t, std::float_t> GetMousePosition() const noexcept override;
		bool IsMouseButtonDown(EMouseButton button) const noexcept override;
		bool IsMouseButtonsDown(EMouseButtonFlag buttonFlags) const noexcept override;
		std::tuple<std::float_t, std::float_t, EMouseButtonFlag> GetMouseState() const noexcept override;
	};
} // namespace tudov

/**
 * @file program/Keyboard.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "KeyCode.hpp"
#include "Log.hpp"
#include "Program/Context.hpp"
#include "ScanCode.hpp"
#include "Util/Definitions.hpp"

#include "sol/forward.hpp"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace tudov::impl
{
	template <typename T>
	struct KeyboardCodeEntry
	{
		std::vector<T> list;
		std::unordered_set<T> set;
	};
} // namespace tudov::impl

namespace tudov
{
	class Context;
	class LuaBindings;
	struct AppEvent;

	struct IKeyboard
	{
		virtual KeyboardID GetKeyboardID() const noexcept = 0;

		virtual bool HandleEvent(AppEvent &appEvent) noexcept = 0;

		virtual bool IsKeyCodeHeld(EKeyCode keyCode, WindowID windowID) noexcept = 0;
		virtual bool IsScanCodeHeld(EScanCode scanCode, WindowID windowID) noexcept = 0;

		TE_FORCEINLINE bool IsKeyCodeHeld(EKeyCode keyCode) noexcept
		{
			return IsKeyCodeHeld(keyCode, 0);
		}
		TE_FORCEINLINE bool IsScanCodeHeld(EScanCode scanCode) noexcept
		{
			return IsScanCodeHeld(scanCode, 0);
		}
	};

	class Keyboard : public IKeyboard, public IContextProvider, private ILogProvider, protected std::enable_shared_from_this<Keyboard>
	{
		friend LuaBindings;

	  private:
		Context &_context;
		KeyboardID _keyboardID;
		std::unordered_map<WindowID, impl::KeyboardCodeEntry<EKeyCode>> _windowHoldingKeyCodes;
		std::unordered_map<WindowID, impl::KeyboardCodeEntry<EScanCode>> _windowHoldingScanCodes;

	  public:
		explicit Keyboard(Context &context, KeyboardID keyboardID) noexcept;
		explicit Keyboard(const Keyboard &) noexcept = default;
		explicit Keyboard(Keyboard &&) noexcept = default;
		Keyboard &operator=(const Keyboard &) noexcept = delete;
		Keyboard &operator=(Keyboard &&) noexcept = delete;
		~Keyboard() noexcept = default;

		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;
		KeyboardID GetKeyboardID() const noexcept override;

		bool HandleEvent(AppEvent &appEvent) noexcept override;

		bool IsKeyCodeHeld(EKeyCode keyCode, WindowID windowID) noexcept override;
		bool IsScanCodeHeld(EScanCode scanCode, WindowID windowID) noexcept override;
		const std::vector<EKeyCode> *GetHeldKeyCodes(WindowID windowID) const noexcept;
		const std::vector<EScanCode> *GetHeldScanCodes(WindowID windowID) const noexcept;

	  private:
		void OnKeyUp(WindowID windowID, EKeyCode keyCode) noexcept;
		void OnKeyDown(WindowID windowID, EKeyCode keyCode) noexcept;

		bool LuaIsKeyCodeHeld(sol::object keyCode, sol::object windowID) noexcept;
		bool LuaIsScanCodeHeld(sol::object scanCode, sol::object windowID) noexcept;
		sol::table LuaListHeldKeyCodes(sol::object windowID) noexcept;
		sol::table LuaListHeldScanCodes(sol::object windowID) noexcept;
	};

	class PrimaryKeyboard final : public Keyboard
	{
	  public:
		PrimaryKeyboard(Context &context) noexcept;

		Log &GetLog() noexcept override;

		bool IsKeyCodeHeld(EKeyCode keyCode, WindowID windowID) noexcept override;
		bool IsScanCodeHeld(EScanCode scanCode, WindowID windowID) noexcept override;
	};
} // namespace tudov

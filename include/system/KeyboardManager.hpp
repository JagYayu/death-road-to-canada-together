/**
 * @file program/KeyboardManager.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "program/EngineComponent.hpp"

#include "sol/forward.hpp"
#include "system/Log.hpp"
#include "util/Definitions.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace tudov
{
	struct IKeyboard;
	struct EventHandleKey;
	class Keyboard;
	class LuaBindings;

	struct IKeyboardManager : public IEngineComponent
	{
		virtual std::shared_ptr<IKeyboard> GetKeyboardAt(std::int32_t index) noexcept = 0;

		virtual std::shared_ptr<IKeyboard> GetKeyboardByID(KeyboardID id) noexcept = 0;

		virtual std::vector<std::shared_ptr<IKeyboard>> *GetKeyboards() noexcept = 0;

		std::shared_ptr<const IKeyboard> GetKeyboardAt(std::int32_t index) const noexcept
		{
			return const_cast<IKeyboardManager *>(this)->GetKeyboardAt(index);
		}

		std::shared_ptr<const IKeyboard> GetKeyboardByID(KeyboardID id) const noexcept
		{
			return const_cast<IKeyboardManager *>(this)->GetKeyboardByID(id);
		}

		const std::vector<std::shared_ptr<IKeyboard>> *GetKeyboards() const noexcept
		{
			return const_cast<IKeyboardManager *>(this)->GetKeyboards();
		}
	};

	class KeyboardManager : public IKeyboardManager, private ILogProvider
	{
		friend LuaBindings;

	  private:
		Context &_context;
		std::vector<std::shared_ptr<Keyboard>> _keyboardList;
		std::unordered_map<std::uint32_t, std::shared_ptr<Keyboard>> _keyboardMap;
		std::vector<std::shared_ptr<IKeyboard>> _keyboards;

	  public:
		explicit KeyboardManager(Context &context) noexcept;

		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		void PreInitialize() noexcept override;
		void PostDeinitialize() noexcept override;

		bool HandleEvent(AppEvent &appEvent) noexcept override;

		std::shared_ptr<IKeyboard> GetKeyboardAt(std::int32_t index) noexcept override;
		std::shared_ptr<IKeyboard> GetKeyboardByID(KeyboardID id) noexcept override;
		std::vector<std::shared_ptr<IKeyboard>> *GetKeyboards() noexcept override;

	  private:
		void AddKeyboard(const std::shared_ptr<Keyboard> &keyboard) noexcept;
		void OnKeyboardDeviceAdded(sol::object e, const EventHandleKey &key) noexcept;

	  private:
		std::shared_ptr<Keyboard> LuaGetKeyboardAt(sol::object index) noexcept;
		std::shared_ptr<Keyboard> LuaGetKeyboardByID(sol::object id) noexcept;
		sol::table LuaListKeyboards() noexcept;
	};
} // namespace tudov

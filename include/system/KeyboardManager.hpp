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

#include "Program/EngineComponent.hpp"

#include "System/Log.hpp"
#include "Util/Definitions.hpp"
#include "sol/forward.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace tudov
{
	struct IKeyboard;
	struct EventHandleKey;
	class Keyboard;
	class PrimaryKeyboard;
	class LuaBindings;

	struct IKeyboardManager : public IEngineComponent
	{
		virtual std::shared_ptr<IKeyboard> GetIKeyboardAt(std::int32_t index) noexcept = 0;

		virtual std::shared_ptr<IKeyboard> GetIKeyboardByID(KeyboardID id) noexcept = 0;

		virtual std::shared_ptr<IKeyboard> GetPrimaryIKeyboard() noexcept = 0;

		virtual std::vector<std::shared_ptr<IKeyboard>> *GetIKeyboards() noexcept = 0;

		std::shared_ptr<const IKeyboard> GetIKeyboardAt(std::int32_t index) const noexcept
		{
			return const_cast<IKeyboardManager *>(this)->GetIKeyboardAt(index);
		}

		std::shared_ptr<const IKeyboard> GetIKeyboardByID(KeyboardID id) const noexcept
		{
			return const_cast<IKeyboardManager *>(this)->GetIKeyboardByID(id);
		}

		const std::vector<std::shared_ptr<IKeyboard>> *GetIKeyboards() const noexcept
		{
			return const_cast<IKeyboardManager *>(this)->GetIKeyboards();
		}
	};

	class KeyboardManager final : public IKeyboardManager, private ILogProvider
	{
		friend LuaBindings;

	  private:
		Context &_context;
		std::shared_ptr<PrimaryKeyboard> _primaryKeyboard;
		std::vector<std::shared_ptr<Keyboard>> _keyboardList;
		std::unordered_map<std::uint32_t, std::shared_ptr<Keyboard>> _keyboardMap;
		std::vector<std::shared_ptr<IKeyboard>> _keyboards;

	  public:
		explicit KeyboardManager(Context &context) noexcept;
		explicit KeyboardManager(const KeyboardManager &) noexcept = delete;
		explicit KeyboardManager(KeyboardManager &&) noexcept = delete;
		KeyboardManager &operator=(const KeyboardManager &) noexcept = delete;
		KeyboardManager &operator=(KeyboardManager &&) noexcept = delete;
		~KeyboardManager() noexcept = default;

		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		void PreInitialize() noexcept override;
		void PostDeinitialize() noexcept override;

		bool HandleEvent(AppEvent &appEvent) noexcept override;

		std::shared_ptr<IKeyboard> GetIKeyboardAt(std::int32_t index) noexcept override;
		std::shared_ptr<IKeyboard> GetIKeyboardByID(KeyboardID id) noexcept override;
		std::shared_ptr<IKeyboard> GetPrimaryIKeyboard() noexcept override;
		std::vector<std::shared_ptr<IKeyboard>> *GetIKeyboards() noexcept override;

	  private:
		void AddKeyboard(const std::shared_ptr<Keyboard> &keyboard) noexcept;
		void OnKeyboardDeviceAdded(sol::object e, const EventHandleKey &key) noexcept;

	  private:
		std::shared_ptr<Keyboard> LuaGetKeyboardAt(sol::object index) noexcept;
		std::shared_ptr<Keyboard> LuaGetKeyboardByID(sol::object id) noexcept;
		std::shared_ptr<Keyboard> LuaGetPrimaryKeyboard() noexcept;
		sol::table LuaListKeyboards() noexcept;
	};
} // namespace tudov

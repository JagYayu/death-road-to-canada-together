/**
 * @file program/MouseManager.h
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
#include "system/Log.hpp"
#include "util/Definitions.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace tudov
{
	struct IMouse;
	class LuaBindings;
	class Mouse;
	class PrimaryMouse;

	struct IMouseManager : public IEngineComponent
	{
		virtual bool IsMouseAvailable(MouseID id) const noexcept = 0;

		virtual std::shared_ptr<IMouse> GetMouseAt(std::int32_t index) noexcept = 0;

		virtual std::shared_ptr<IMouse> GetMouseByID(MouseID id) noexcept = 0;

		virtual std::shared_ptr<IMouse> GetPrimaryMouse() noexcept = 0;

		inline std::shared_ptr<IMouse> GetMouseAt(std::int32_t index) const noexcept
		{
			return const_cast<IMouseManager *>(this)->GetMouseAt(index);
		}

		inline std::shared_ptr<IMouse> GetMouseByID(MouseID id) const noexcept
		{
			return const_cast<IMouseManager *>(this)->GetMouseByID(id);
		}
	};

	class MouseManager final : public IMouseManager, private ILogProvider
	{
		friend LuaBindings;

	  private:
		Context &_context;

		std::shared_ptr<PrimaryMouse> _primaryMouse;
		std::vector<std::shared_ptr<Mouse>> _mouseList;
		std::unordered_map<MouseID, std::shared_ptr<Mouse>> _mouseMap;

	  public:
		explicit MouseManager(Context &context) noexcept;

		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		bool IsMouseAvailable(MouseID id) const noexcept override;
		std::shared_ptr<IMouse> GetMouseAt(std::int32_t index) noexcept override;
		std::shared_ptr<IMouse> GetMouseByID(MouseID id) noexcept override;
		std::shared_ptr<IMouse> GetPrimaryMouse() noexcept override;

	  protected:
		void AddMouse(std::shared_ptr<Mouse> mouse);

	  private:
		std::shared_ptr<Mouse> LuaGetMouseAt(std::int32_t index) noexcept;
		std::shared_ptr<Mouse> LuaGetMouseByID(MouseID id) noexcept;
		std::shared_ptr<Mouse> LuaGetPrimaryMouse() noexcept;
	};
} // namespace tudov

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
#include "util/Definitions.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace tudov
{
	class Mouse;

	struct IMouseManager : public IEngineComponent
	{
		virtual std::shared_ptr<Mouse> GetMouseAt(std::int32_t index) noexcept = 0;
		virtual std::shared_ptr<Mouse> GetMouseByID(MouseID id) noexcept = 0;
	};

	class MouseManager : public IMouseManager
	{
	  private:
		Context &_context;

		std::vector<std::shared_ptr<Mouse>> _mouseList;
		std::unordered_map<MouseID, std::shared_ptr<Mouse>> _mouseMap;

	  public:
		explicit MouseManager(Context &context) noexcept;

		Context &GetContext() noexcept override;

		std::shared_ptr<Mouse> GetMouseAt(std::int32_t index) noexcept override;
		std::shared_ptr<Mouse> GetMouseByID(MouseID id) noexcept override;
	};
} // namespace tudov

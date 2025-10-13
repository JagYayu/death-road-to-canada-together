/**
 * @file debug/DebugMods.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "DebugElement.hpp"

namespace tudov
{
	class IWindow;

	class DebugMods : public IDebugElement, private ILogProvider
	{
	  protected:
		//

	  public:
		inline static constexpr std::string_view Name() noexcept
		{
			return "Mods";
		}

	  public:
		std::weak_ptr<IWindow> window;

	  protected:
		//

	  public:
		explicit DebugMods() noexcept = default;
		~DebugMods() noexcept;

		Log &GetLog() noexcept override;

		std::string_view GetName() noexcept override;
		void UpdateAndRender(IWindow &window) noexcept override;
	};
} // namespace tudov

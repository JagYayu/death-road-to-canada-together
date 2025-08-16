/**
 * @file resource/GameScripts.hpp
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

namespace tudov
{
	struct IGameScripts : public IEngineComponent
	{
	};

	class GameScripts : public IGameScripts
	{
	  private:
		Context &_context;

	  public:
		explicit GameScripts(Context &context) noexcept;

		Context &GetContext() noexcept override;
		void Initialize() noexcept override;
		void Deinitialize() noexcept override;
	};
} // namespace tudov

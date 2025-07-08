#pragma once

#include "program/EngineComponent.h"

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

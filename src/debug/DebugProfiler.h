#include "IDebugElement.h"

namespace tudov
{
	class EventManager;

	class DebugProfiler : public IDebugElement
	{
		UInt64 _prevPrefCounter;

	  public:
		EventManager &eventManager;

		DebugProfiler(EventManager &eventManager) noexcept;

		StringView GetName() noexcept override;
		void UpdateAndRender() noexcept override;
	};
} // namespace tudov
#pragma once

namespace tudov
{
	/**
	 * @brief Tudov engine base class
	 */
	struct Application
	{
		virtual ~Application() noexcept = default;

		virtual void Initialize() noexcept = 0;
		virtual bool Tick() noexcept = 0;
		virtual void Event(void *event) noexcept = 0;
		virtual void Deinitialize() noexcept = 0;
	};
} // namespace tudov
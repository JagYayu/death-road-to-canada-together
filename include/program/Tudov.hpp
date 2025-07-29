#pragma once

#include <memory>
#include <string_view>

namespace tudov
{
	struct Application;
	struct MainArgs;

	void FatalError(std::string_view errorMessage) noexcept;

	std::weak_ptr<Application> GetApplication() noexcept;

	const MainArgs &GetMainArgs() noexcept;

	void InitMainArgs(const MainArgs &args);

	template <typename T>
	std::weak_ptr<T> GetApplication() noexcept
	{
		auto &&app = GetApplication();
		if (app.expired())
		{
			return nullptr;
		}
		return std::dynamic_pointer_cast<T>(app);
	}
} // namespace tudov

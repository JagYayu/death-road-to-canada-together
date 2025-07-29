#pragma once

#include <memory>
#include <string_view>

namespace tudov
{
	struct Application;
	struct MainArgs;

	struct Tudov
	{
		explicit Tudov() noexcept = delete;
		~Tudov() noexcept = delete;

		static void FatalError(std::string_view errorMessage) noexcept;

		static std::shared_ptr<Application> GetApplication() noexcept;

		static const MainArgs &GetMainArgs() noexcept;

		static void InitMainArgs(const MainArgs &args);

		template <typename T>
		static std::shared_ptr<T> GetApplication() noexcept
		{
			return std::dynamic_pointer_cast<T>(GetApplication());
		}
	};
} // namespace tudov

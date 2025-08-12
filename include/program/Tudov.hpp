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

		static void ReleaseApplication() noexcept;

		static const MainArgs &GetMainArgs() noexcept;

		static void InitMainArgs(int argc, char **argv);

		// static std::uint64_t GetSystemTimeNS() noexcept;
		// static void Delay(std::uint32_t ns) noexcept;

		template <typename T>
		static std::shared_ptr<T> GetApplication() noexcept
		{
			return std::dynamic_pointer_cast<T>(GetApplication());
		}

		// template <typename... TArgs>
		// static bool WaitFor(std::uint32_t ns, std::function<bool(TArgs...)> function, TArgs... args)
		// {
		// 	std::uint64_t end = GetSystemTimeNS() + ns;
		// 	while (!function(std::forward(args)...))
		// 	{
		// 		if (GetSystemTimeNS() >= end)
		// 		{
		// 			return false;
		// 		}
		// 		Delay(1);
		// 	}
		// 	return true;
		// }
	};
} // namespace tudov

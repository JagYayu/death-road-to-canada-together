/**
 * @file util/.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

namespace tudov
{
	struct NonCopyable
	{
		NonCopyable() noexcept = default;
		~NonCopyable() noexcept = default;

		explicit NonCopyable(const NonCopyable &) noexcept = delete;
		NonCopyable &operator=(const NonCopyable &) = delete;
	};

	struct NonMovable
	{
		NonMovable() noexcept = default;
		~NonMovable() noexcept = default;

		explicit NonMovable(NonMovable &&) noexcept = delete;
		NonMovable &operator=(NonMovable &&) noexcept = delete;
	};

	struct NonCopyableMovable : public NonCopyable, public NonMovable
	{
		NonCopyableMovable() noexcept = default;
		~NonCopyableMovable() noexcept = default;
	};
} // namespace tudov

/**
 * @file misc/Text.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "resource/Resource.hpp"

#include <cstddef>
#include <string_view>
#include <vector>

namespace tudov
{
	class Text : public IResource
	{
	  private:
		std::string_view _path;
		std::string_view _view;

	  public:
		explicit Text() noexcept = default;
		explicit Text(const Text &) noexcept = default;
		explicit Text(Text &&) noexcept = default;
		Text &operator=(const Text &) noexcept = default;
		Text &operator=(Text &&) noexcept = default;
		~Text() noexcept override = default;

		explicit Text(std::string_view path, const std::vector<std::byte> &bytes) noexcept;

		std::string_view GetFilePath() const noexcept override;

		operator std::string_view() const noexcept;

		std::string_view View() const noexcept;
	};
} // namespace tudov

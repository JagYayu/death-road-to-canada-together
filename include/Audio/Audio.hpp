/**
 * @file audio/Audio.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include <span>
#include <string_view>
#include <vector>

namespace tudov
{
	class Audio
	{
	  private:
		std::span<const std::byte> _bytes;

	  public:
		explicit Audio(std::string_view path, const std::vector<std::byte> &bytes) noexcept;
		explicit Audio(const Audio &) noexcept = default;
		explicit Audio(Audio &&) noexcept = default;
		Audio &operator=(const Audio &) noexcept = default;
		Audio &operator=(Audio &&) noexcept = default;
		~Audio() noexcept;
	};
} // namespace tudov

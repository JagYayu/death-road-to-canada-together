#pragma once

#include "resource/Resource.hpp"

#include <cstdint>
#include <span>
#include <vector>

namespace tudov
{
	class Binaries : public IResource
	{
	  protected:
		std::string_view _path;
		std::span<const std::byte> _bytes;

	  public:
		explicit Binaries(std::string_view path, const std::vector<std::byte> &bytes) noexcept;
		~Binaries() noexcept override = default;

		std::string_view GetFilePath() const noexcept override;

		std::span<const std::byte> GetBytes() const noexcept;

		std::uint64_t GetSize() const noexcept;
	};
} // namespace tudov

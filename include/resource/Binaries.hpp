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
		std::vector<std::byte> _bytes;

	  public:
		explicit Binaries(std::span<std::byte> bytes) noexcept
		    : _bytes(bytes.begin(), bytes.end())
		{
		}

		explicit Binaries(const std::vector<std::byte> &bytes) noexcept
		    : _bytes(bytes)
		{
		}

		explicit Binaries(std::vector<std::byte> &&bytes) noexcept
		    : _bytes(std::move(bytes))
		{
		}

		~Binaries() noexcept override
		{
		}

		std::span<std::byte> ReadBytes() override
		{
			return _bytes;
		}

		std::vector<std::byte> &GetBytes() noexcept
		{
			return _bytes;
		}

		const std::vector<std::byte> &GetBytes() const noexcept
		{
			return _bytes;
		}

		std::span<std::byte> GetSpan() noexcept
		{
			return _bytes;
		}

		std::span<const std::byte> GetSpan() const noexcept
		{
			return _bytes;
		}

		std::uint64_t GetSize() const noexcept
		{
			return _bytes.size();
		}
	};
} // namespace tudov

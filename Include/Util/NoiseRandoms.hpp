/**
 * @file Util/NoiseRandoms.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <cmath>

namespace tudov
{
	class LuaBindings;

	struct INoiseRandom
	{
		virtual std::int32_t GetSeed() noexcept = 0;

		virtual std::float_t GetFrequency() noexcept = 0;

		virtual void SetSeed(std::int32_t value) noexcept = 0;

		virtual void SetFrequency(std::float_t value) noexcept = 0;

		virtual std::float_t Noise2(std::float_t x, std::float_t y) noexcept = 0;

		virtual std::float_t Noise3(std::float_t x, std::float_t y, std::float_t z) noexcept = 0;
	};

	class PerlinNoiseRandom final : public INoiseRandom
	{
		friend LuaBindings;

	  private:
		void *_data;
		std::int32_t _seed;
		std::float_t _frequency;

	  public:
		explicit PerlinNoiseRandom() noexcept;
		explicit PerlinNoiseRandom(std::int32_t seed) noexcept;
		explicit PerlinNoiseRandom(std::float_t _frequency) noexcept;
		explicit PerlinNoiseRandom(std::int32_t seed, std::float_t _frequency) noexcept;
		~PerlinNoiseRandom() noexcept;

		std::int32_t GetSeed() noexcept override;
		std::float_t GetFrequency() noexcept override;
		void SetSeed(std::int32_t value) noexcept override;
		void SetFrequency(std::float_t value) noexcept override;
		std::float_t Noise2(std::float_t x, std::float_t y) noexcept override;
		std::float_t Noise3(std::float_t x, std::float_t y, std::float_t z) noexcept override;
	};
} // namespace tudov

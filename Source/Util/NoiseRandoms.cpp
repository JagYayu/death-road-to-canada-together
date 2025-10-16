/**
 * @file Util/NoiseRandoms.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Util/NoiseRandoms.hpp"

#include "FastNoiseLite.hpp"
#include <cmath>

using namespace tudov;

PerlinNoiseRandom::PerlinNoiseRandom() noexcept
    : PerlinNoiseRandom(1337, 0.01f)
{
}

PerlinNoiseRandom::PerlinNoiseRandom(std::int32_t seed) noexcept
    : PerlinNoiseRandom(seed, 0.01f)
{
}

PerlinNoiseRandom::PerlinNoiseRandom(std::float_t frequency) noexcept
    : PerlinNoiseRandom(1337, frequency)
{
}

PerlinNoiseRandom::PerlinNoiseRandom(std::int32_t seed, std::float_t frequency) noexcept
    : _seed(seed),
      _frequency(frequency)
{
	FastNoiseLite *data = new FastNoiseLite(_seed);
	data->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	data->SetFrequency(frequency);
	_data = data;
}

PerlinNoiseRandom::~PerlinNoiseRandom() noexcept
{
	delete static_cast<FastNoiseLite *>(_data);
}

std::int32_t PerlinNoiseRandom::GetSeed() noexcept
{
	return _seed;
}

std::float_t PerlinNoiseRandom::GetFrequency() noexcept
{
	return _frequency;
}

void PerlinNoiseRandom::SetSeed(std::int32_t value) noexcept
{
	_seed = value;
	static_cast<FastNoiseLite *>(_data)->SetSeed(value);
}

void PerlinNoiseRandom::SetFrequency(std::float_t value) noexcept
{
	_frequency = value;
	static_cast<FastNoiseLite *>(_data)->SetFrequency(value);
}

std::float_t PerlinNoiseRandom::Noise1(std::float_t x) noexcept
{
	return static_cast<FastNoiseLite *>(_data)->GetNoise(x, static_cast<std::float_t>(_seed));
}

std::float_t PerlinNoiseRandom::Noise2(std::float_t x, std::float_t y) noexcept
{
	return static_cast<FastNoiseLite *>(_data)->GetNoise(x, y);
}

std::float_t PerlinNoiseRandom::Noise3(std::float_t x, std::float_t y, std::float_t z) noexcept
{
	return static_cast<FastNoiseLite *>(_data)->GetNoise(x, y, z);
}

std::float_t PerlinNoiseRandom::Float1(std::float_t x, std::float_t min, std::float_t max) noexcept
{
	return min + (Noise1(x) + 1.0f) * 0.5f * (max - min);
}

std::float_t PerlinNoiseRandom::Float2(std::float_t x, std::float_t y, std::float_t min, std::float_t max) noexcept
{
	return min + (Noise2(x, y) + 1.0f) * 0.5f * (max - min);
}

std::float_t PerlinNoiseRandom::Float3(std::float_t x, std::float_t y, std::float_t z, std::float_t min, std::float_t max) noexcept
{
	return min + (Noise3(x, y, z) + 1.0f) * 0.5f * (max - min);
}

static constexpr std::float_t granularity = 2 ^ 24 - 1;

std::int32_t PerlinNoiseRandom::Int1(std::float_t x, std::int32_t min, std::int32_t max) noexcept
{
	return min + static_cast<std::int32_t>((Noise1(x) + 1.0f) * 0.5f * granularity) % (max - min + 1);
}

std::int32_t PerlinNoiseRandom::Int2(std::float_t x, std::float_t y, std::int32_t min, std::int32_t max) noexcept
{
	return min + static_cast<std::int32_t>((Noise2(x, y) + 1.0f) * 0.5f * granularity) % (max - min + 1);
}

std::int32_t PerlinNoiseRandom::Int3(std::float_t x, std::float_t y, std::float_t z, std::int32_t min, std::int32_t max) noexcept
{
	return min + static_cast<std::int32_t>((Noise3(x, y, z) + 1.0f) * 0.5f * granularity) % (max - min + 1);
}
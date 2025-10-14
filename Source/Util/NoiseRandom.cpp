/**
 * @file Util/NoiseRandom.cpp
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

std::float_t PerlinNoiseRandom::Noise2(std::float_t x, std::float_t y) noexcept
{
	return static_cast<FastNoiseLite *>(_data)->GetNoise(x, y);
}

std::float_t PerlinNoiseRandom::Noise3(std::float_t x, std::float_t y, std::float_t z) noexcept
{
	return static_cast<FastNoiseLite *>(_data)->GetNoise(x, y, z);
}

/**
 * @file util/Utils.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "util/Utils.hpp"

#include "zconf.h"
#include "zlib.h"

using namespace tudov;

std::vector<std::byte> tudov::CompressData(const std::vector<std::byte> &input)
{
	if (input.empty())
	{
		return {};
	}

	uLongf compressedSize = compressBound(static_cast<uLong>(input.size()));
	std::vector<std::byte> output(compressedSize);
	int res = compress2(
	    reinterpret_cast<Bytef *>(output.data()), &compressedSize,
	    reinterpret_cast<const Bytef *>(input.data()), static_cast<uLong>(input.size()),
	    Z_BEST_COMPRESSION);

	if (res != Z_OK)
	{
		throw std::runtime_error("minizip compress2 failed: " + std::to_string(res));
	}
	output.resize(compressedSize);

	return output;
}

std::vector<std::byte> tudov::DecompressData(const std::vector<std::byte> &input, std::size_t originalSize)
{
	if (input.empty())
	{
		return {};
	}

	std::vector<std::byte> output(originalSize);
	uLongf destLen = static_cast<uLongf>(originalSize);
	int res = uncompress(
	    reinterpret_cast<Bytef *>(output.data()), &destLen,
	    reinterpret_cast<const Bytef *>(input.data()), static_cast<uLong>(input.size()));

	if (res != Z_OK)
	{
		throw std::runtime_error("uncompress failed with code " + std::to_string(res));
	}

	return output;
}

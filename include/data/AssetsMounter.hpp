/**
 * @file data/AssetsMounter.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Util/Micros.hpp"

namespace tudov
{
	class Context;

	/**
	 * It maybe similar to `AssetsManager`, but this is a static class, and only provide a functionality to load contents from directories.
	 * Don't support managing or live reloading assets, basically load external assets once during application initialization, and that's all.
	 */
	class AssetsMounter
	{
		TE_STATIC_CLASS(AssetsMounter);

	  public:
		static void MountDirectory(Context &context);
	};
} // namespace tudov

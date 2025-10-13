/**
 * @file data/GlobalStorageLocation.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Util/Micros.hpp"

#include <cstdint>
#include <filesystem>

namespace tudov
{
	enum class EGlobalStorageLocation : std::uint8_t
	{
		/**
		 * Entire disk location, readonly, e.g. "C:/".
		 */
		Readonly = 0,
		/**
		 * Application storage location, e.g. "D:/Program Files/<AppName>".
		 */
		Application = 1,
		/**
		 * User storage location, e.g. "C:/Users/Admin/AppData/Roaming/<AppOrganization>/<AppName>".
		 */
		User = 2,
		/**
		 * Other storage locations, represent virtual locations so they don't have actual physical storage location
		 * and just represent the locations in Virtual File System.
		 */
		Other = 3,
	};

	/**
	 * Static Class
	 * Used to check the real directory paths on physical disk.
	 * Only PC platforms are accessible.
	 */
	class GlobalStorageLocation
	{
	  public:
		TE_STATIC_CLASS(GlobalStorageLocation);

		static bool IsAccessible() noexcept;

		/**
		 * Return the file physical path of specific Global Storage Location.
		 *
		 * @param[in]  location
		 *
		 * @return
		 */
		static std::filesystem::path GetPath(EGlobalStorageLocation location) noexcept;
	};
} // namespace tudov

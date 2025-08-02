#include "util/Micros.hpp"
#include <filesystem>
namespace tudov
{
	enum class EGlobalStorageLocation
	{
		Readonly = 0,
		Application = 1,
		User = 2,
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

		static std::filesystem::path GetPath(EGlobalStorageLocation location) noexcept;
	};
} // namespace tudov

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

	class GlobalStorageLocation
	{
	  public:
		TE_STATIC_CLASS(GlobalStorageLocation);

		static std::filesystem::path GetPath(EGlobalStorageLocation location) noexcept;
	};
} // namespace tudov

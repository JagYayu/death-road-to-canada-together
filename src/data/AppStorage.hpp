#pragma once

#include "Storage.hpp"

namespace tudov
{
	struct IAppStorage : public IStorage
	{
		virtual ~IAppStorage() noexcept = default;
	};

	class AppStorage : public IAppStorage
	{
	  public:
		explicit AppStorage() noexcept;
		~AppStorage() noexcept override = default;
	};
} // namespace tudov

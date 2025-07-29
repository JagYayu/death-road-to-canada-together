#pragma once

namespace tudov
{
	struct IAssetBundle
	{
		virtual ~IAssetBundle() noexcept = default;
	};

	class AssetBundle : public IAssetBundle
	{
		explicit AssetBundle();
		~AssetBundle() noexcept override = default;
	};
} // namespace tudov

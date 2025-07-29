#pragma once

#include "program/EngineComponent.hpp"

#include "util/Version.hpp"

#include <string>
#include <vector>

namespace tudov
{
	struct IAssetBundles : public IEngineComponent
	{
		struct Info
		{
			std::string path;
			std::string name;
			Version version;
		};

		virtual ~IAssetBundles() noexcept = default;

		virtual const std::vector<Info> &GetFounded() const noexcept;
		virtual const std::vector<Info> &GetLoaded() const noexcept;
	};

	class AssetBundles : public IAssetBundles
	{
	  private:
		std::vector<Info> _founded;
		std::vector<Info> _loaded;

	  public:
		explicit AssetBundles() noexcept = default;
		~AssetBundles() noexcept override = default;

		void Initialize() noexcept override;
		void Deinitialize() noexcept override;

		const std::vector<Info> &GetFounded() const noexcept override;
		const std::vector<Info> &GetLoaded() const noexcept override;
	};
} // namespace tudov

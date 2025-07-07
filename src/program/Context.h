#pragma once

#include <memory>

namespace tudov
{
	class Engine;
	class EngineConfig;
	class ImageManager;
	class FontManager;
	class IEventManager;
	class IModManager;
	class IScriptEngine;
	class IScriptLoader;
	class IScriptProvider;

	class Context
	{
		friend Engine;

	  private:
		Engine *_engine;

	  public:
		explicit Context(Engine *engine = nullptr) noexcept;
		~Context() noexcept = default;

	  public:
		Engine &GetEngine();
		const Engine &GetEngine() const;
		EngineConfig &GetEngineConfig();
		const EngineConfig &GetEngineConfig() const;
		ImageManager &GetImageManager();
		const ImageManager &GetImageManager() const;
		FontManager &GetFontManager();
		const FontManager &GetFontManager() const;

		std::weak_ptr<IEventManager> GetEventManager();
		std::weak_ptr<IModManager> GetModManager();
		std::weak_ptr<IScriptEngine> GetScriptEngine();
		std::weak_ptr<IScriptLoader> GetScriptLoader();
		std::weak_ptr<IScriptProvider> GetScriptProvider();

		inline std::weak_ptr<const IEventManager> GetEventManager() const
		{
			return const_cast<Context *>(this)->GetEventManager();
		}

		inline std::weak_ptr<const IModManager> GetModManager() const
		{
			return const_cast<Context *>(this)->GetModManager();
		}

		inline std::weak_ptr<const IScriptEngine> GetScriptEngine() const
		{
			return const_cast<Context *>(this)->GetScriptEngine();
		}

		inline std::weak_ptr<const IScriptLoader> GetScriptLoader() const
		{
			return const_cast<Context *>(this)->GetScriptLoader();
		}

		inline std::weak_ptr<const IScriptProvider> GetScriptProvider() const
		{
			return const_cast<Context *>(this)->GetScriptProvider();
		}
	};
} // namespace tudov

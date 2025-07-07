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
		std::weak_ptr<const IEventManager> GetEventManager() const;
		std::weak_ptr<IModManager> GetModManager();
		std::weak_ptr<const IModManager> GetModManager() const;
		std::weak_ptr<IScriptEngine> GetScriptEngine();
		std::weak_ptr<const IScriptEngine> GetScriptEngine() const;
		std::weak_ptr<IScriptLoader> GetScriptLoader();
		std::weak_ptr<const IScriptLoader> GetScriptLoader() const;
		std::weak_ptr<IScriptProvider> GetScriptProvider();
		std::weak_ptr<const IScriptProvider> GetScriptProvider() const;
	};
} // namespace tudov

#pragma once

#include <memory>

namespace tudov
{
	class Config;
	class Engine;
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
		Config &GetConfig();
		const Config &GetEngineConfig() const;
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

	class Engine;
	class IEventManager;
	class IModManager;
	class IScriptEngine;
	class IScriptLoader;
	class IScriptProvider;

	struct IContextProvider
	{
		virtual ~IContextProvider() noexcept = default;
		
		virtual Context &GetContext() noexcept = 0;

		const Context &GetContext() const noexcept
		{
			return const_cast<IContextProvider *>(this)->GetContext();
		}

		inline Engine &GetEngine() noexcept
		{
			return GetContext().GetEngine();
		}

		inline const Engine &GetEngine() const noexcept
		{
			return GetContext().GetEngine();
		}

		inline Config &GetEngineConfig() noexcept
		{
			return GetContext().GetConfig();
		}

		inline const Config &GetEngineConfig() const noexcept
		{
			return GetContext().GetEngineConfig();
		}

		inline FontManager &GetFontManager() noexcept
		{
			return GetContext().GetFontManager();
		}

		inline const FontManager &GetFontManager() const noexcept
		{
			return GetContext().GetFontManager();
		}

		inline ImageManager &GetImageManager() noexcept
		{
			return GetContext().GetImageManager();
		}

		inline const ImageManager &GetImageManager() const noexcept
		{
			return GetContext().GetImageManager();
		}

		inline std::shared_ptr<IEventManager> GetEventManager() noexcept
		{
			auto &&ptr = GetContext().GetEventManager();
			return ptr.expired() ? nullptr : ptr.lock();
		}

		inline std::shared_ptr<const IEventManager> GetEventManager() const noexcept
		{
			auto &&ptr = GetContext().GetEventManager();
			return ptr.expired() ? nullptr : ptr.lock();
		}

		inline std::shared_ptr<IModManager> GetModManager() noexcept
		{
			auto &&ptr = GetContext().GetModManager();
			return ptr.expired() ? nullptr : ptr.lock();
		}

		inline std::shared_ptr<const IModManager> GetModManager() const noexcept
		{
			auto &&ptr = GetContext().GetModManager();
			return ptr.expired() ? nullptr : ptr.lock();
		}

		inline std::shared_ptr<IScriptEngine> GetScriptEngine() noexcept
		{
			auto &&ptr = GetContext().GetScriptEngine();
			return ptr.expired() ? nullptr : ptr.lock();
		}

		inline std::shared_ptr<const IScriptEngine> GetScriptEngine() const noexcept
		{
			auto &&ptr = GetContext().GetScriptEngine();
			return ptr.expired() ? nullptr : ptr.lock();
		}

		inline std::shared_ptr<IScriptLoader> GetScriptLoader() noexcept
		{
			auto &&ptr = GetContext().GetScriptLoader();
			return ptr.expired() ? nullptr : ptr.lock();
		}

		inline std::shared_ptr<const IScriptLoader> GetScriptLoader() const noexcept
		{
			auto &&ptr = GetContext().GetScriptLoader();
			return ptr.expired() ? nullptr : ptr.lock();
		}

		inline std::shared_ptr<IScriptProvider> GetScriptProvider() noexcept
		{
			auto &&ptr = GetContext().GetScriptProvider();
			return ptr.expired() ? nullptr : ptr.lock();
		}

		inline std::shared_ptr<const IScriptProvider> GetScriptProvider() const noexcept
		{
			auto &&ptr = GetContext().GetScriptProvider();
			return ptr.expired() ? nullptr : ptr.lock();
		}
	};
} // namespace tudov

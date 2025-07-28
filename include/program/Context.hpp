#pragma once

namespace tudov
{
	struct IEventManager;
	struct ILuaAPI;
	struct IModManager;
	struct IScriptEngine;
	struct IScriptLoader;
	struct IScriptProvider;
	struct IStorageManager;
	class Config;
	class Engine;
	class MainArgs;
	class ImageManager;
	class FontManager;
	class TextManager;

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
		Config &GetConfig();
		MainArgs &GetMainArgs();
		IEventManager &GetEventManager();
		ILuaAPI &GetLuaAPI();
		IModManager &GetModManager();
		IScriptEngine &GetScriptEngine();
		IScriptLoader &GetScriptLoader();
		IScriptProvider &GetScriptProvider();
		IStorageManager &GetStorageManager();
		ImageManager &GetImageManager();
		FontManager &GetFontManager();
		TextManager &GetTextManager();

		inline const Engine &GetEngine() const
		{
			return const_cast<Context *>(this)->GetEngine();
		}

		inline const Config &GetConfig() const
		{
			return const_cast<Context *>(this)->GetConfig();
		}

		inline const MainArgs &GetMainArgs() const
		{
			return const_cast<Context *>(this)->GetMainArgs();
		}

		inline const IEventManager &GetEventManager() const
		{
			return This()->GetEventManager();
		}

		inline ILuaAPI &GetLuaAPI() const
		{
			return This()->GetLuaAPI();
		}

		inline const IModManager &GetModManager() const
		{
			return This()->GetModManager();
		}

		inline const IScriptEngine &GetScriptEngine() const
		{
			return This()->GetScriptEngine();
		}

		inline const IScriptLoader &GetScriptLoader() const
		{
			return This()->GetScriptLoader();
		}

		inline const IScriptProvider &GetScriptProvider() const
		{
			return This()->GetScriptProvider();
		}

		inline const IStorageManager &GetStorageManager() const
		{
			return This()->GetStorageManager();
		}

		inline const ImageManager &GetImageManager() const
		{
			return const_cast<Context *>(this)->GetImageManager();
		}

		inline const FontManager &GetFontManager() const
		{
			return const_cast<Context *>(this)->GetFontManager();
		}

		inline const TextManager &GetTextManager() const
		{
			return const_cast<Context *>(this)->GetTextManager();
		}

	  private:
		inline Context *This() const noexcept
		{
			return const_cast<Context *>(this);
		}
	};

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

		inline Config &GetConfig() noexcept
		{
			return GetContext().GetConfig();
		}

		inline const Config &GetConfig() const noexcept
		{
			return GetContext().GetConfig();
		}

		inline MainArgs &GetMainArgs() noexcept
		{
			return GetContext().GetMainArgs();
		}

		inline const MainArgs &GetMainArgs() const noexcept
		{
			return GetContext().GetMainArgs();
		}

		inline IEventManager &GetEventManager() noexcept
		{
			return GetContext().GetEventManager();
		}

		inline const IEventManager &GetEventManager() const noexcept
		{
			return GetContext().GetEventManager();
		}

		inline ILuaAPI &GetLuaAPI() noexcept
		{
			return GetContext().GetLuaAPI();
		}

		inline const ILuaAPI &GetLuaAPI() const noexcept
		{
			return GetContext().GetLuaAPI();
		}

		inline IModManager &GetModManager() noexcept
		{
			return GetContext().GetModManager();
		}

		inline const IModManager &GetModManager() const noexcept
		{
			return GetContext().GetModManager();
		}

		inline IScriptEngine &GetScriptEngine() noexcept
		{
			return GetContext().GetScriptEngine();
		}

		inline const IScriptEngine &GetScriptEngine() const noexcept
		{
			return GetContext().GetScriptEngine();
		}

		inline IScriptLoader &GetScriptLoader() noexcept
		{
			return GetContext().GetScriptLoader();
		}

		inline const IScriptLoader &GetScriptLoader() const noexcept
		{
			return GetContext().GetScriptLoader();
		}

		inline IScriptProvider &GetScriptProvider() noexcept
		{
			return GetContext().GetScriptProvider();
		}

		inline const IScriptProvider &GetScriptProvider() const noexcept
		{
			return GetContext().GetScriptProvider();
		}

		inline IStorageManager &GetStorageManager() noexcept
		{
			return GetContext().GetStorageManager();
		}

		inline const IStorageManager &GetStorageManager() const noexcept
		{
			return GetContext().GetStorageManager();
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

		inline TextManager &GetTextManager() noexcept
		{
			return GetContext().GetTextManager();
		}

		inline const TextManager &GetTextManager() const noexcept
		{
			return GetContext().GetTextManager();
		}
	};
} // namespace tudov

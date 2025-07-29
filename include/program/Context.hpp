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
	class ImageResources;
	class FontResources;
	class TextResources;

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
		IEventManager &GetEventManager();
		ILuaAPI &GetLuaAPI();
		IModManager &GetModManager();
		IScriptEngine &GetScriptEngine();
		IScriptLoader &GetScriptLoader();
		IScriptProvider &GetScriptProvider();
		IStorageManager &GetStorageManager();
		ImageResources &GetImageResources();
		FontResources &GetFontResources();
		TextResources &GetTextResources();

		inline const Engine &GetEngine() const
		{
			return const_cast<Context *>(this)->GetEngine();
		}

		inline const Config &GetConfig() const
		{
			return const_cast<Context *>(this)->GetConfig();
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

		inline const ImageResources &GetImageResources() const
		{
			return const_cast<Context *>(this)->GetImageResources();
		}

		inline const FontResources &GetFontResources() const
		{
			return const_cast<Context *>(this)->GetFontResources();
		}

		inline const TextResources &GetTextResources() const
		{
			return const_cast<Context *>(this)->GetTextResources();
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

		inline FontResources &GetFontResources() noexcept
		{
			return GetContext().GetFontResources();
		}

		inline const FontResources &GetFontResources() const noexcept
		{
			return GetContext().GetFontResources();
		}

		inline ImageResources &GetImageResources() noexcept
		{
			return GetContext().GetImageResources();
		}

		inline const ImageResources &GetImageResources() const noexcept
		{
			return GetContext().GetImageResources();
		}

		inline TextResources &GetTextResources() noexcept
		{
			return GetContext().GetTextResources();
		}

		inline const TextResources &GetTextResources() const noexcept
		{
			return GetContext().GetTextResources();
		}
	};
} // namespace tudov

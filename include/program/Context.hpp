#pragma once

namespace tudov
{
	struct IEventManager;
	struct IGlobalStorageManager;
	struct ILuaAPI;
	struct IModManager;
	struct IScriptEngine;
	struct IScriptLoader;
	struct IScriptProvider;
	class Config;
	class Engine;
	class MainArgs;
	class GlobalResourcesCollection;
	class VirtualFileSystem;
	class WindowManager;

	/**
	 * Context of tudov engine.
	 */
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
		IGlobalStorageManager &GetGlobalStorageManager();
		GlobalResourcesCollection &GetGlobalResourcesCollection();
		VirtualFileSystem &GetVirtualFileSystem();
		WindowManager &GetWindowManager();

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

		inline const IGlobalStorageManager &GetGlobalStorageManager() const
		{
			return This()->GetGlobalStorageManager();
		}

		inline const GlobalResourcesCollection &GetGlobalResourcesCollection() const
		{
			return const_cast<Context *>(this)->GetGlobalResourcesCollection();
		}

		inline const VirtualFileSystem &GetVirtualFileSystem() const
		{
			return const_cast<Context *>(this)->GetVirtualFileSystem();
		}

		inline const WindowManager &GetWindowManager() const
		{
			return const_cast<Context *>(this)->GetWindowManager();
		}

	  private:
		inline Context *This() const noexcept
		{
			return const_cast<Context *>(this);
		}
	};

	/**
	 * Can be inherited by other class, will provide engine component getter functions for child class.
	 */
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

		inline IGlobalStorageManager &GetGlobalStorageManager() noexcept
		{
			return GetContext().GetGlobalStorageManager();
		}

		inline const IGlobalStorageManager &GetGlobalStorageManager() const noexcept
		{
			return GetContext().GetGlobalStorageManager();
		}

		inline GlobalResourcesCollection &GetGlobalResourcesCollection() noexcept
		{
			return GetContext().GetGlobalResourcesCollection();
		}

		inline const GlobalResourcesCollection &GetGlobalResourcesCollection() const noexcept
		{
			return GetContext().GetGlobalResourcesCollection();
		}

		inline VirtualFileSystem &GetVirtualFileSystem() noexcept
		{
			return GetContext().GetVirtualFileSystem();
		}

		inline const VirtualFileSystem &GetVirtualFileSystem() const noexcept
		{
			return GetContext().GetVirtualFileSystem();
		}

		WindowManager &GetWindowManager() noexcept
		{
			return GetContext().GetWindowManager();
		}

		const WindowManager &GetWindowManager() const noexcept
		{
			return GetContext().GetWindowManager();
		}
	};
} // namespace tudov

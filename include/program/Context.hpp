/**
 * @file program/Context.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "util/Micros.hpp"

namespace tudov
{
	struct IEventManager;
	struct IGlobalStorageManager;
	struct ILuaAPI;
	struct IModManager;
	struct IScriptEngine;
	struct IScriptErrors;
	struct IScriptLoader;
	struct IScriptProvider;
	struct ILocalization;
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
		[[deprecated("Use `Tudov::GetConfig` instead")]]
		Config &GetConfig();
		IEventManager &GetEventManager();
		ILuaAPI &GetLuaAPI();
		IModManager &GetModManager();
		IScriptEngine &GetScriptEngine();
		IScriptErrors &GetScriptErrors();
		IScriptLoader &GetScriptLoader();
		IScriptProvider &GetScriptProvider();
		IGlobalStorageManager &GetGlobalStorageManager();
		ILocalization &GetLocalization();
		GlobalResourcesCollection &GetGlobalResourcesCollection();
		VirtualFileSystem &GetVirtualFileSystem();
		WindowManager &GetWindowManager();

		TE_FORCEINLINE const Engine &GetEngine() const
		{
			return const_cast<Context *>(this)->GetEngine();
		}

		TE_FORCEINLINE const Config &GetConfig() const
		{
			return const_cast<Context *>(this)->GetConfig();
		}

		TE_FORCEINLINE const IEventManager &GetEventManager() const
		{
			return This()->GetEventManager();
		}

		TE_FORCEINLINE ILuaAPI &GetLuaAPI() const
		{
			return This()->GetLuaAPI();
		}

		TE_FORCEINLINE const IModManager &GetModManager() const
		{
			return This()->GetModManager();
		}

		TE_FORCEINLINE const IScriptEngine &GetScriptEngine() const
		{
			return This()->GetScriptEngine();
		}

		TE_FORCEINLINE const IScriptErrors &GetScriptErrors() const
		{
			return This()->GetScriptErrors();
		}

		TE_FORCEINLINE const IScriptLoader &GetScriptLoader() const
		{
			return This()->GetScriptLoader();
		}

		TE_FORCEINLINE const IScriptProvider &GetScriptProvider() const
		{
			return This()->GetScriptProvider();
		}

		TE_FORCEINLINE const IGlobalStorageManager &GetGlobalStorageManager() const
		{
			return This()->GetGlobalStorageManager();
		}

		TE_FORCEINLINE const ILocalization &GetLocalization() const
		{
			return This()->GetLocalization();
		}

		TE_FORCEINLINE const GlobalResourcesCollection &GetGlobalResourcesCollection() const
		{
			return const_cast<Context *>(this)->GetGlobalResourcesCollection();
		}

		TE_FORCEINLINE const VirtualFileSystem &GetVirtualFileSystem() const
		{
			return const_cast<Context *>(this)->GetVirtualFileSystem();
		}

		TE_FORCEINLINE const WindowManager &GetWindowManager() const
		{
			return const_cast<Context *>(this)->GetWindowManager();
		}

	  private:
		TE_FORCEINLINE Context *This() const noexcept
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

		TE_FORCEINLINE const Context &GetContext() const noexcept
		{
			return const_cast<IContextProvider *>(this)->GetContext();
		}

		TE_FORCEINLINE Engine &GetEngine() noexcept
		{
			return GetContext().GetEngine();
		}

		TE_FORCEINLINE const Engine &GetEngine() const noexcept
		{
			return GetContext().GetEngine();
		}

		TE_FORCEINLINE Config &GetConfig() noexcept
		{
			return GetContext().GetConfig();
		}

		TE_FORCEINLINE const Config &GetConfig() const noexcept
		{
			return GetContext().GetConfig();
		}

		TE_FORCEINLINE IEventManager &GetEventManager() noexcept
		{
			return GetContext().GetEventManager();
		}

		TE_FORCEINLINE const IEventManager &GetEventManager() const noexcept
		{
			return GetContext().GetEventManager();
		}

		TE_FORCEINLINE ILuaAPI &GetLuaAPI() noexcept
		{
			return GetContext().GetLuaAPI();
		}

		TE_FORCEINLINE const ILuaAPI &GetLuaAPI() const noexcept
		{
			return GetContext().GetLuaAPI();
		}

		TE_FORCEINLINE IModManager &GetModManager() noexcept
		{
			return GetContext().GetModManager();
		}

		TE_FORCEINLINE const IModManager &GetModManager() const noexcept
		{
			return GetContext().GetModManager();
		}

		TE_FORCEINLINE IScriptEngine &GetScriptEngine() noexcept
		{
			return GetContext().GetScriptEngine();
		}

		TE_FORCEINLINE const IScriptEngine &GetScriptEngine() const noexcept
		{
			return GetContext().GetScriptEngine();
		}

		TE_FORCEINLINE IScriptErrors &GetScriptErrors() noexcept
		{
			return GetContext().GetScriptErrors();
		}

		TE_FORCEINLINE const IScriptErrors &GetScriptErrors() const noexcept
		{
			return GetContext().GetScriptErrors();
		}

		TE_FORCEINLINE IScriptLoader &GetScriptLoader() noexcept
		{
			return GetContext().GetScriptLoader();
		}

		TE_FORCEINLINE const IScriptLoader &GetScriptLoader() const noexcept
		{
			return GetContext().GetScriptLoader();
		}

		TE_FORCEINLINE IScriptProvider &GetScriptProvider() noexcept
		{
			return GetContext().GetScriptProvider();
		}

		TE_FORCEINLINE const IScriptProvider &GetScriptProvider() const noexcept
		{
			return GetContext().GetScriptProvider();
		}

		TE_FORCEINLINE IGlobalStorageManager &GetGlobalStorageManager() noexcept
		{
			return GetContext().GetGlobalStorageManager();
		}

		ILocalization &GetLocalization() noexcept
		{
			return GetContext().GetLocalization();
		}

		const ILocalization &GetLocalization() const noexcept
		{
			return GetContext().GetLocalization();
		}

		TE_FORCEINLINE const IGlobalStorageManager &GetGlobalStorageManager() const noexcept
		{
			return GetContext().GetGlobalStorageManager();
		}

		TE_FORCEINLINE GlobalResourcesCollection &GetGlobalResourcesCollection() noexcept
		{
			return GetContext().GetGlobalResourcesCollection();
		}

		TE_FORCEINLINE const GlobalResourcesCollection &GetGlobalResourcesCollection() const noexcept
		{
			return GetContext().GetGlobalResourcesCollection();
		}

		TE_FORCEINLINE VirtualFileSystem &GetVirtualFileSystem() noexcept
		{
			return GetContext().GetVirtualFileSystem();
		}

		TE_FORCEINLINE const VirtualFileSystem &GetVirtualFileSystem() const noexcept
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

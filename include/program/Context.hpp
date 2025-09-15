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
	struct ILuaBindings;
	struct IModManager;
	struct INetworkManager;
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
	class IKeyboardManager;
	class IMouseManager;

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
		[[nodiscard]] Engine &GetEngine();
		[[nodiscard]] IEventManager &GetEventManager();
		[[nodiscard]] ILuaBindings &GetLuaBindings();
		[[nodiscard]] IModManager &GetModManager();
		[[nodiscard]] IScriptEngine &GetScriptEngine();
		[[nodiscard]] IScriptErrors &GetScriptErrors();
		[[nodiscard]] IScriptLoader &GetScriptLoader();
		[[nodiscard]] IScriptProvider &GetScriptProvider();
		[[nodiscard]] IGlobalStorageManager &GetGlobalStorageManager();
		[[nodiscard]] ILocalization &GetLocalization();
		[[nodiscard]] INetworkManager &GetNetworkManager();
		[[nodiscard]] IKeyboardManager &GetKeyboardManager();
		[[nodiscard]] IMouseManager &GetMouseManager();
		[[nodiscard]] GlobalResourcesCollection &GetGlobalResourcesCollection();
		[[nodiscard]] VirtualFileSystem &GetVirtualFileSystem();
		[[nodiscard]] WindowManager &GetWindowManager();

		[[nodiscard]] TE_FORCEINLINE const Engine &GetEngine() const
		{
			return const_cast<Context *>(this)->GetEngine();
		}

		[[nodiscard]] TE_FORCEINLINE const IEventManager &GetEventManager() const
		{
			return This()->GetEventManager();
		}

		[[nodiscard]] TE_FORCEINLINE ILuaBindings &GetLuaBindings() const
		{
			return This()->GetLuaBindings();
		}

		[[nodiscard]] TE_FORCEINLINE const IModManager &GetModManager() const
		{
			return This()->GetModManager();
		}

		[[nodiscard]] TE_FORCEINLINE const IScriptEngine &GetScriptEngine() const
		{
			return This()->GetScriptEngine();
		}

		[[nodiscard]] TE_FORCEINLINE const IScriptErrors &GetScriptErrors() const
		{
			return This()->GetScriptErrors();
		}

		[[nodiscard]] TE_FORCEINLINE const IScriptLoader &GetScriptLoader() const
		{
			return This()->GetScriptLoader();
		}

		[[nodiscard]] TE_FORCEINLINE const IScriptProvider &GetScriptProvider() const
		{
			return This()->GetScriptProvider();
		}

		[[nodiscard]] TE_FORCEINLINE const IGlobalStorageManager &GetGlobalStorageManager() const
		{
			return This()->GetGlobalStorageManager();
		}

		[[nodiscard]] TE_FORCEINLINE const ILocalization &GetLocalization() const
		{
			return This()->GetLocalization();
		}

		[[nodiscard]] TE_FORCEINLINE const INetworkManager &GetNetworkManager() const
		{
			return const_cast<Context *>(this)->GetNetworkManager();
		}

		[[nodiscard]] TE_FORCEINLINE const GlobalResourcesCollection &GetGlobalResourcesCollection() const
		{
			return const_cast<Context *>(this)->GetGlobalResourcesCollection();
		}

		[[nodiscard]] TE_FORCEINLINE const VirtualFileSystem &GetVirtualFileSystem() const
		{
			return const_cast<Context *>(this)->GetVirtualFileSystem();
		}

		[[nodiscard]] TE_FORCEINLINE const WindowManager &GetWindowManager() const
		{
			return const_cast<Context *>(this)->GetWindowManager();
		}

		[[nodiscard]] const IKeyboardManager &GetKeyboardManager() const
		{
			return const_cast<Context *>(this)->GetKeyboardManager();
		}

		[[nodiscard]] const IMouseManager &GetMouseManager() const
		{
			return const_cast<Context *>(this)->GetMouseManager();
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

		[[nodiscard]] virtual Context &GetContext() noexcept = 0;

		[[nodiscard]] TE_FORCEINLINE const Context &GetContext() const noexcept
		{
			return const_cast<IContextProvider *>(this)->GetContext();
		}

		[[nodiscard]] TE_FORCEINLINE Engine &GetEngine() noexcept
		{
			return GetContext().GetEngine();
		}

		[[nodiscard]] TE_FORCEINLINE const Engine &GetEngine() const noexcept
		{
			return GetContext().GetEngine();
		}

		[[nodiscard]] TE_FORCEINLINE IEventManager &GetEventManager() noexcept
		{
			return GetContext().GetEventManager();
		}

		[[nodiscard]] TE_FORCEINLINE const IEventManager &GetEventManager() const noexcept
		{
			return GetContext().GetEventManager();
		}

		[[nodiscard]] TE_FORCEINLINE ILuaBindings &GetLuaBindings() noexcept
		{
			return GetContext().GetLuaBindings();
		}

		[[nodiscard]] TE_FORCEINLINE const ILuaBindings &GetLuaBindings() const noexcept
		{
			return GetContext().GetLuaBindings();
		}

		[[nodiscard]] TE_FORCEINLINE IModManager &GetModManager() noexcept
		{
			return GetContext().GetModManager();
		}

		[[nodiscard]] TE_FORCEINLINE const IModManager &GetModManager() const noexcept
		{
			return GetContext().GetModManager();
		}

		[[nodiscard]] TE_FORCEINLINE IScriptEngine &GetScriptEngine() noexcept
		{
			return GetContext().GetScriptEngine();
		}

		[[nodiscard]] TE_FORCEINLINE const IScriptEngine &GetScriptEngine() const noexcept
		{
			return GetContext().GetScriptEngine();
		}

		[[nodiscard]] TE_FORCEINLINE IScriptErrors &GetScriptErrors() noexcept
		{
			return GetContext().GetScriptErrors();
		}

		[[nodiscard]] TE_FORCEINLINE const IScriptErrors &GetScriptErrors() const noexcept
		{
			return GetContext().GetScriptErrors();
		}

		[[nodiscard]] TE_FORCEINLINE IScriptLoader &GetScriptLoader() noexcept
		{
			return GetContext().GetScriptLoader();
		}

		[[nodiscard]] TE_FORCEINLINE const IScriptLoader &GetScriptLoader() const noexcept
		{
			return GetContext().GetScriptLoader();
		}

		[[nodiscard]] TE_FORCEINLINE IScriptProvider &GetScriptProvider() noexcept
		{
			return GetContext().GetScriptProvider();
		}

		[[nodiscard]] TE_FORCEINLINE const IScriptProvider &GetScriptProvider() const noexcept
		{
			return GetContext().GetScriptProvider();
		}

		[[nodiscard]] TE_FORCEINLINE IGlobalStorageManager &GetGlobalStorageManager() noexcept
		{
			return GetContext().GetGlobalStorageManager();
		}

		[[nodiscard]] ILocalization &GetLocalization() noexcept
		{
			return GetContext().GetLocalization();
		}

		[[nodiscard]] const ILocalization &GetLocalization() const noexcept
		{
			return GetContext().GetLocalization();
		}

		[[nodiscard]] INetworkManager &GetNetworkManager() noexcept
		{
			return GetContext().GetNetworkManager();
		}

		[[nodiscard]] const INetworkManager &GetNetworkManager() const noexcept
		{
			return GetContext().GetNetworkManager();
		}

		[[nodiscard]] TE_FORCEINLINE const IGlobalStorageManager &GetGlobalStorageManager() const noexcept
		{
			return GetContext().GetGlobalStorageManager();
		}

		[[nodiscard]] TE_FORCEINLINE GlobalResourcesCollection &GetGlobalResourcesCollection() noexcept
		{
			return GetContext().GetGlobalResourcesCollection();
		}

		[[nodiscard]] TE_FORCEINLINE const GlobalResourcesCollection &GetGlobalResourcesCollection() const noexcept
		{
			return GetContext().GetGlobalResourcesCollection();
		}

		[[nodiscard]] TE_FORCEINLINE VirtualFileSystem &GetVirtualFileSystem() noexcept
		{
			return GetContext().GetVirtualFileSystem();
		}

		[[nodiscard]] TE_FORCEINLINE const VirtualFileSystem &GetVirtualFileSystem() const noexcept
		{
			return GetContext().GetVirtualFileSystem();
		}

		[[nodiscard]] WindowManager &GetWindowManager() noexcept
		{
			return GetContext().GetWindowManager();
		}

		[[nodiscard]] const WindowManager &GetWindowManager() const noexcept
		{
			return GetContext().GetWindowManager();
		}

		[[nodiscard]] IKeyboardManager &GetKeyboardManager()
		{
			return GetContext().GetKeyboardManager();
		}

		[[nodiscard]] IMouseManager &GetMouseManager()
		{
			return GetContext().GetMouseManager();
		}
	};
} // namespace tudov

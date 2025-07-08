#pragma once

#include "Context.h"

namespace tudov
{
	class Context;
	class Engine;
	class IEventManager;
	class IModManager;
	class IScriptEngine;
	class IScriptLoader;
	class IScriptProvider;

	struct IEngineComponent
	{
		virtual Context &GetContext() noexcept = 0;

		virtual void Initialize() noexcept
		{
		}

		virtual void Deinitialize() noexcept
		{
		}

		const Context &GetContext() const noexcept
		{
			return const_cast<IEngineComponent *>(this)->GetContext();
		}

		inline Engine &GetEngine() noexcept
		{
			return GetContext().GetEngine();
		}

		inline const Engine &GetEngine() const noexcept
		{
			return GetContext().GetEngine();
		}

		inline EngineConfig &GetEngineConfig() noexcept
		{
			return GetContext().GetEngineConfig();
		}

		inline const EngineConfig &GetEngineConfig() const noexcept
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

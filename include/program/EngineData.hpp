/**
 * @file program/EngineComponents.h
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <memory>
#include <vector>

namespace tudov
{
	class Engine;
	class Context;
	struct IEngineComponent;
	struct ILuaBindings;
	struct IGlobalStorageManager;
	struct ILocalization;
	class VirtualFileSystem;
	class GlobalResourcesCollection;
	struct IAssetsManager;
	class WindowManager;
	struct INetworkManager;
	struct IKeyboardManager;
	struct IMouseManager;
	struct IModManager;
	struct IEventManager;
	struct IGameScripts;
	struct IScriptEngine;
	struct IScriptErrors;
	struct IScriptLoader;
	struct IScriptProvider;

	class EngineData
	{
		friend Engine;
		friend Context;

		std::shared_ptr<ILuaBindings> _luaBindings;
		std::shared_ptr<IGlobalStorageManager> _globalStorageManager;
		std::shared_ptr<ILocalization> _localization;
		std::shared_ptr<VirtualFileSystem> _virtualFileSystem;

		std::vector<std::shared_ptr<IEngineComponent>> _components;

		std::shared_ptr<GlobalResourcesCollection> _globalResourcesCollection;
		std::shared_ptr<IAssetsManager> _assetsManager;
		std::shared_ptr<WindowManager> _windowManager;
		std::shared_ptr<INetworkManager> _networkManager;
		std::shared_ptr<IKeyboardManager> _keyboardManager;
		std::shared_ptr<IMouseManager> _mouseManager;
		std::shared_ptr<IModManager> _modManager;
		std::shared_ptr<IEventManager> _eventManager;
		std::shared_ptr<IGameScripts> _gameScripts;
		std::shared_ptr<IScriptEngine> _scriptEngine;
		std::shared_ptr<IScriptErrors> _scriptErrors;
		std::shared_ptr<IScriptLoader> _scriptLoader;
		std::shared_ptr<IScriptProvider> _scriptProvider;

	  public:
		explicit EngineData(Context &context) noexcept;
		explicit EngineData(const EngineData &) noexcept = delete;
		explicit EngineData(EngineData &&) noexcept = delete;
		EngineData &operator=(const EngineData &) noexcept = delete;
		EngineData &operator=(EngineData &&) noexcept = delete;
		~EngineData() noexcept = default;
	};
} // namespace tudov

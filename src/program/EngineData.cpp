/**
 * @file program/EngineComponents.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "program/EngineData.hpp"
#include "data/AssetsManager.hpp"
#include "data/GlobalStorageManager.hpp"
#include "data/VirtualFileSystem.hpp"
#include "event/EventManager.hpp"
#include "i18n/Localization.hpp"
#include "mod/LuaAPI.hpp"
#include "mod/ModManager.hpp"
#include "mod/ScriptErrors.hpp"
#include "mod/ScriptLoader.hpp"
#include "network/NetworkManager.hpp"
#include "program/Context.hpp"
#include "program/EngineComponent.hpp"
#include "program/Window.hpp"
#include "program/WindowManager.hpp"
#include "resource/FontResources.hpp"
#include "resource/GlobalResourcesCollection.hpp"
#include "resource/ImageResources.hpp"
#include "resource/TextResources.hpp"
#include "scripts/GameScripts.hpp"
#include "system/Log.hpp"

using namespace tudov;

EngineData::EngineData(Context &context) noexcept
    : _luaAPI(std::make_shared<LuaAPI>()),
      _globalStorageManager(std::make_shared<GlobalStorageManager>()),
      _localization(std::make_shared<Localization>()),
      _virtualFileSystem(std::make_shared<VirtualFileSystem>(context)),
      _globalResourcesCollection(std::make_shared<GlobalResourcesCollection>(context)),
      _assetsManager(std::make_shared<AssetsManager>(context)),
      _windowManager(std::make_shared<WindowManager>(context)),
      _networkManager(std::make_shared<NetworkManager>(context)),
      _modManager(std::make_shared<ModManager>(context)),
      _scriptProvider(std::make_shared<ScriptProvider>(context)),
      _scriptLoader(std::make_shared<ScriptLoader>(context)),
      _scriptEngine(std::make_shared<ScriptEngine>(context)),
      _scriptErrors(std::make_shared<ScriptErrors>(context)),
      _eventManager(std::make_shared<EventManager>(context)),
      _gameScripts(std::make_shared<GameScripts>(context))
{
	_components = {
	    _globalResourcesCollection,
	    _assetsManager,
	    _modManager,
	    _networkManager,
	    _eventManager,
	    _gameScripts,
	    _scriptProvider,
	    _scriptLoader,
	    _scriptEngine,
	    _scriptErrors,
	};
}

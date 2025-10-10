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

#include "Program/EngineData.hpp"

#include "Data/AssetsManager.hpp"
#include "Data/GlobalStorageManager.hpp"
#include "Data/VirtualFileSystem.hpp"
#include "Event/EventManager.hpp"
#include "I18n/Localization.hpp"
#include "mod/LuaBindings.hpp"
#include "mod/ModManager.hpp"
#include "mod/ScriptErrors.hpp"
#include "mod/ScriptLoader.hpp"
#include "Network/NetworkManager.hpp"
#include "Program/Context.hpp"
#include "Program/EngineComponent.hpp"
#include "Program/Window.hpp"
#include "Program/WindowManager.hpp"
#include "Resource/FontResources.hpp"
#include "Resource/GlobalResourcesCollection.hpp"
#include "Resource/ImageResources.hpp"
#include "Resource/TextResources.hpp"
#include "scripts/GameScripts.hpp"
#include "System/KeyboardManager.hpp"
#include "System/Log.hpp"
#include "System/MouseManager.hpp"

#include <memory>
#include <unordered_map>

using namespace tudov;

EngineData::EngineData(Context &context) noexcept
    : _luaBindings(std::make_shared<LuaBindings>()),
      _globalStorageManager(std::make_shared<GlobalStorageManager>()),
      _localization(std::make_shared<Localization>()),
      _virtualFileSystem(std::make_shared<VirtualFileSystem>(context)),
      _globalResourcesCollection(std::make_shared<GlobalResourcesCollection>(context)),
      _assetsManager(std::make_shared<AssetsManager>(context)),
      _windowManager(std::make_shared<WindowManager>(context)),
      _networkManager(std::make_shared<NetworkManager>(context)),
      _keyboardManager(std::make_shared<KeyboardManager>(context)),
      _mouseManager(std::make_shared<MouseManager>(context)),
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
	    _windowManager,
	    _keyboardManager,
	    _mouseManager,
	    _eventManager,
	    _gameScripts,
	    _scriptProvider,
	    _scriptLoader,
	    _scriptEngine,
	    _scriptErrors,
	};

	std::unordered_map<IEngineComponent *, std::int32_t> fallbackSequences{};

	for (std::int32_t i = 0; i < _components.size(); ++i)
	{
		fallbackSequences[_components.at(i).get()] = i + 1;
	}

	std::sort(_components.begin(), _components.end(), [&fallbackSequences](std::shared_ptr<IEngineComponent> &l, std::shared_ptr<IEngineComponent> &r) -> bool
	{
		std::int32_t lss = l->GetSortingSequence();
		std::int32_t rss = l->GetSortingSequence();
		if (lss != rss)
		{
			return lss < rss;
		}
		else
		{
			return fallbackSequences.at(l.get()) < fallbackSequences.at(r.get());
		}
	});
}

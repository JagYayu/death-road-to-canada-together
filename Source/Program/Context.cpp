/**
 * @file program/Context.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Program/Context.hpp"

#include "I18n/Localization.hpp"
#include "Program/Engine.hpp"
#include "Program/EngineData.hpp"
#include "Resource/GlobalResourcesCollection.hpp"
#include "System/KeyboardManager.hpp"
#include "System/MouseManager.hpp"
#include "Util/MicrosImpl.hpp"

#include <stdexcept>

using namespace tudov;

static constexpr const char *errorMessage = "Engine pointer is nullptr";

Context::Context(Engine *engine) noexcept
    : _engine(engine)
{
}

Engine &Context::GetEngine()
{
	if (_engine == nullptr) [[unlikely]]
	{
		throw std::runtime_error("Engine does not exist");
	}
	return *_engine;
}

IEventManager &Context::GetEventManager()
{
	return *GetEngine()._data->_eventManager;
}

IGameScripts &Context::GetGameScripts()
{
	return *GetEngine()._data->_gameScripts;
}

IKeyboardManager &Context::GetKeyboardManager()
{
	return *GetEngine()._data->_keyboardManager;
}

ILuaBindings &Context::GetLuaBindings()
{
	return *GetEngine()._data->_luaBindings;
}

IModManager &Context::GetModManager()
{
	return *GetEngine()._data->_modManager;
}

IMouseManager &Context::GetMouseManager()
{
	return *GetEngine()._data->_mouseManager;
}

IScriptEngine &Context::GetScriptEngine()
{
	return *GetEngine()._data->_scriptEngine;
}

IScriptErrors &Context::GetScriptErrors()
{
	return *GetEngine()._data->_scriptErrors;
}

IScriptLoader &Context::GetScriptLoader()
{
	return *GetEngine()._data->_scriptLoader;
}

IScriptProvider &Context::GetScriptProvider()
{
	return *GetEngine()._data->_scriptProvider;
}

IGlobalStorageManager &Context::GetGlobalStorageManager()
{
	return *GetEngine()._data->_globalStorageManager;
}

ILocalization &Context::GetLocalization()
{
	return *GetEngine()._data->_localization;
}

GlobalResourcesCollection &Context::GetGlobalResourcesCollection()
{
	return *GetEngine()._data->_globalResourcesCollection;
}

VirtualFileSystem &Context::GetVirtualFileSystem()
{
	return *GetEngine()._data->_virtualFileSystem;
}

WindowManager &Context::GetWindowManager()
{
	return *GetEngine()._data->_windowManager;
}

INetworkManager &Context::GetNetworkManager()
{
	return *GetEngine()._data->_networkManager;
}

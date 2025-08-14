#include "program/Context.hpp"
#include "i18n/Localization.hpp"
#include "program/Engine.hpp"

#include "resource/GlobalResourcesCollection.hpp"
#include "util/MicrosImpl.hpp"

#include <stdexcept>

using namespace tudov;

Context::Context(Engine *engine) noexcept
    : _engine(engine)
{
}

static constexpr const char *errorMessage = "Engine pointer is nullptr";

Engine &Context::GetEngine()
{
	if (!_engine) [[unlikely]]
	{
		throw std::runtime_error(errorMessage);
	}
	return *_engine;
}

Config &Context::GetConfig()
{
	return *GetEngine()._config;
}

IEventManager &Context::GetEventManager()
{
	return *GetEngine()._eventManager;
}

ILuaAPI &Context::GetLuaAPI()
{
	return *GetEngine()._luaAPI;
}

IModManager &Context::GetModManager()
{
	return *GetEngine()._modManager;
}

IScriptEngine &Context::GetScriptEngine()
{
	return *GetEngine()._scriptEngine;
}

IScriptErrors &Context::GetScriptErrors()
{
	return *GetEngine()._scriptErrors;
}

IScriptLoader &Context::GetScriptLoader()
{
	return *GetEngine()._scriptLoader;
}

IScriptProvider &Context::GetScriptProvider()
{
	return *GetEngine()._scriptProvider;
}

IGlobalStorageManager &Context::GetGlobalStorageManager()
{
	return *GetEngine()._globalStorageManager;
}

ILocalization &Context::GetLocalization()
{
	return *GetEngine()._localization;
}

GlobalResourcesCollection &Context::GetGlobalResourcesCollection()
{
	return *GetEngine()._globalResourcesCollection;
}

VirtualFileSystem &Context::GetVirtualFileSystem()
{
	return *GetEngine()._virtualFileSystem;
}

WindowManager &Context::GetWindowManager()
{
	return *GetEngine()._windowManager;
}

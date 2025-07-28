#include "program/Context.hpp"
#include "program/Engine.hpp"

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

MainArgs &Context::GetMainArgs()
{
	return *GetEngine()._mainArgs;
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

IScriptLoader &Context::GetScriptLoader()
{
	return *GetEngine()._scriptLoader.get();
}

IScriptProvider &Context::GetScriptProvider()
{
	return *GetEngine()._scriptProvider.get();
}

IStorageManager &Context::GetStorageManager()
{
	return *GetEngine()._storageManager.get();
}

ImageManager &Context::GetImageManager()
{
	return *GetEngine()._imageManager;
}

FontManager &Context::GetFontManager()
{
	return *GetEngine()._fontManager;
}

TextManager &Context::GetTextManager()
{
	return *GetEngine()._textManager.get();
}

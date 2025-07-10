#include "Context.hpp"

#include "Engine.hpp"
#include "util/MicrosImpl.hpp"
#include <stdexcept>

using namespace tudov;

Context::Context(Engine *engine) noexcept
    : _engine(engine)
{
}

static constexpr const char *errorMessage = "engine pointer is nullptr";

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
	return GetEngine()._config;
}

ImageManager &Context::GetImageManager()
{
	return GetEngine()._imageManager;
}

FontManager &Context::GetFontManager()
{
	return GetEngine()._fontManager;
}

IEventManager *Context::GetEventManager()
{
	return GetEngine()._eventManager.get();
}

ILuaAPI *Context::GetLuaAPI()
{
	return GetEngine()._luaAPI.get();
}

IModManager *Context::GetModManager()
{
	return GetEngine()._modManager.get();
}

IScriptEngine *Context::GetScriptEngine()
{
	return GetEngine()._scriptEngine.get();
}

IScriptLoader *Context::GetScriptLoader()
{
	return GetEngine()._scriptLoader.get();
}

IScriptProvider *Context::GetScriptProvider()
{
	return GetEngine()._scriptProvider.get();
}

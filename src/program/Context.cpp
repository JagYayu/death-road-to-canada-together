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

EngineConfig &Context::GetEngineConfig()
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

std::weak_ptr<IEventManager> Context::GetEventManager()
{
	return GetEngine()._eventManager;
}

std::weak_ptr<IModManager> Context::GetModManager()
{
	return GetEngine()._modManager;
}

std::weak_ptr<IScriptEngine> Context::GetScriptEngine()
{
	return GetEngine()._scriptEngine;
}

std::weak_ptr<IScriptLoader> Context::GetScriptLoader()
{
	return GetEngine()._scriptLoader;
}

std::weak_ptr<IScriptProvider> Context::GetScriptProvider()
{
	return GetEngine()._scriptProvider;
}

// TUDOV_GEN_GETTER_REF(Engine, Context::GetEngine, *_engine, noexcept);
// TUDOV_GEN_GETTER_REF(EngineConfig, Context::GetEngineConfig, _engine->_config, noexcept);
// TUDOV_GEN_GETTER_REF(ImageManager, Context::GetImageManager, _engine->_imageManager, noexcept);
// TUDOV_GEN_GETTER_REF(FontManager, Context::GetFontManager, _engine->_fontManager, noexcept);

// TUDOV_GEN_GETTER_SMART_PTR(std::weak_ptr, IEventManager, Context::GetEventManager, _engine->_eventManager, noexcept);
// TUDOV_GEN_GETTER_SMART_PTR(std::weak_ptr, IModManager, Context::GetModManager, _engine->_modManager, noexcept);
// TUDOV_GEN_GETTER_SMART_PTR(std::weak_ptr, IScriptEngine, Context::GetScriptEngine, _engine->_scriptEngine, noexcept);
// TUDOV_GEN_GETTER_SMART_PTR(std::weak_ptr, IScriptLoader, Context::GetScriptLoader, _engine->_scriptLoader, noexcept);
// TUDOV_GEN_GETTER_SMART_PTR(std::weak_ptr, IScriptProvider, Context::GetScriptProvider, _engine->_scriptProvider, noexcept);

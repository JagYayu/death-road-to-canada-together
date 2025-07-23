#include "resource/ImageManager.hpp"

using namespace tudov;

ImageManager::ImageManager() noexcept
{
}

void ImageManager::InstallToScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept
{
}

void ImageManager::UninstallFromScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept
{
}

ImageID ImageManager::LuaGetID(const sol::string_view &image) noexcept
{
	return GetResourceID(std::string_view(image));
}

sol::string_view ImageManager::LuaGetPath(ImageID id) noexcept
{
	return GetResourcePath(id);
}

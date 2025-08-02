#include "resource/ImageResources.hpp"

using namespace tudov;

ImageResources::ImageResources() noexcept
{
}

void ImageResources::InstallToScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept
{
}

void ImageResources::UninstallFromScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept
{
}

ImageID ImageResources::LuaGetID(const std::string_view &image) noexcept
{
	return GetResourceID(std::string_view(image));
}

std::string_view ImageResources::LuaGetPath(ImageID id) noexcept
{
	return GetResourcePath(id);
}

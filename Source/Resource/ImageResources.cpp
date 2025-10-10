/**
 * @file resource/ImageResources.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Resource/ImageResources.hpp"

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

ImageID ImageResources::LuaGetID(std::string_view image) noexcept
{
	return GetResourceID(image);
}

std::string_view ImageResources::LuaGetPath(ImageID id) noexcept
{
	return GetResourcePath(id);
}

#pragma once

#include "ResourceManager.hpp"

#include <string_view>

namespace tudov
{
	// template <>
	// inline ResourceManager<Shader>::ResourceManager() noexcept
	//     : _log(Log::Get("ShaderManager"))
	// {
	// }

	// class ScriptEngine;

	// class ShaderManager : public ResourceManager<Shader>
	// {
	//   public:
	// 	ShaderManager() noexcept;

	// 	void InstallToScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;
	// 	void UninstallFromScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;
	// };
} // namespace tudov

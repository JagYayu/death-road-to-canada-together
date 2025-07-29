#pragma once

#include "Resources.hpp"

#include <string_view>

namespace tudov
{
	// template <>
	// inline Resources<Shader>::Resources() noexcept
	//     : _log(Log::Get("ShaderManager"))
	// {
	// }

	// class ScriptEngine;

	// class ShaderManager : public Resources<Shader>
	// {
	//   public:
	// 	ShaderManager() noexcept;

	// 	void InstallToScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;
	// 	void UninstallFromScriptEngine(std::string_view name, ScriptEngine &scriptEngine) noexcept;
	// };
} // namespace tudov

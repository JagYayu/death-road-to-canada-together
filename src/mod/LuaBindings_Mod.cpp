/**
 * @file mod/LuaBindings_Mod.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "mod/LuaBindings.hpp"

#include "mod/ModManager.hpp"
#include "mod/ScriptErrors.hpp"
#include "mod/ScriptLoader.hpp"

using namespace tudov;

#define TE_ENUM(Class, ...)     lua.new_enum<Class>(#Class, __VA_ARGS__)
#define TE_USERTYPE(Class, ...) lua.new_usertype<Class>(#Class, __VA_ARGS__)

void LuaBindings::InstallMod(sol::state &lua, Context &context) noexcept
{

	TE_USERTYPE(ModConfig,
	            "author", &ModConfig::author,
	            "description", &ModConfig::description,
	            "distribution", &ModConfig::distribution,
	            "name", &ModConfig::name,
	            "namespace", &ModConfig::namespace_,
	            "uid", &ModConfig::uid);

	TE_USERTYPE(ModManager,
	            "loadMods", &ModManager::LoadModsDeferred,
	            "unloadMods", &ModManager::UnloadModsDeferred);

	// TE_USERTYPE(ScriptEngine);

	TE_USERTYPE(ScriptErrors,
	            "addLoadtimeError", &ScriptErrors::LuaAddLoadtimeError,
	            "addRuntimeError", &ScriptErrors::LuaAddRuntimeError,
	            "clearLoadtimeErrors", &ScriptErrors::ClearLoadtimeErrors,
	            "clearRuntimeErrors", &ScriptErrors::ClearRuntimeErrors,
	            "hasLoadtimeErrors", &ScriptErrors::HasLoadtimeError,
	            "hasRuntimeErrors", &ScriptErrors::HasRuntimeError);

	TE_USERTYPE(ScriptLoader,
	            "addReverseDependency", &ScriptLoader::LuaAddReverseDependency,
	            "getLoadingScriptID", &ScriptLoader::GetLoadingScriptID,
	            "getLoadingScriptName", &ScriptLoader::GetLoadingScriptName);

	TE_USERTYPE(ScriptProvider,
	            "getScriptTextID", &ScriptProvider::GetScriptTextID);

	lua["mods"] = &dynamic_cast<ModManager &>(context.GetModManager());
	lua["scriptEngine"] = &dynamic_cast<ScriptEngine &>(context.GetScriptEngine());
	lua["scriptErrors"] = &dynamic_cast<ScriptErrors &>(context.GetScriptErrors());
	lua["scriptLoader"] = &dynamic_cast<ScriptLoader &>(context.GetScriptLoader());
	lua["scriptProvider"] = &dynamic_cast<ScriptProvider &>(context.GetScriptProvider());

	lua.set_function("getModConfig", [this, &context](sol::string_view modUID) -> ModConfig *
	{
		std::shared_ptr<Mod> mod = context.GetModManager().FindLoadedMod(modUID);
		return mod == nullptr ? nullptr : &mod->GetConfig();
	});
}

/**
 * @file Mod/LuaBindings_Mod.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "mod/LuaBindings.hpp"

#include "Util/MicrosImpl.hpp"
#include "Util/Version.hpp"
#include "mod/ModManager.hpp"
#include "mod/ScriptErrors.hpp"
#include "mod/ScriptLoader.hpp"
#include "sol/property.hpp"

using namespace tudov;

void LuaBindings::InstallMod(sol::state &lua, Context &context) noexcept
{
	TE_LB_ENUM(
	    EModDistribution,
	    {
	        {"ClientOnly", EModDistribution::ClientOnly},
	        {"ServerOnly", EModDistribution::ServerOnly},
	        {"Universal", EModDistribution::Universal},
	    });

	TE_LB_USERTYPE(
	    Mod,
	    "getConfig", &Mod::GetConfig,
	    "shouldScriptLoad", &Mod::ShouldScriptLoad);

	TE_LB_USERTYPE(
	    ModConfig,
	    "author", &ModConfig::author,
	    "description", &ModConfig::description,
	    "distribution", &ModConfig::distribution,
	    "name", &ModConfig::name,
	    "namespace", &ModConfig::namespace_,
	    "uid", &ModConfig::uid,
	    "version", sol::readonly_property(&ModConfig::LuaGetVersion));

	TE_LB_USERTYPE(
	    ModManager,
	    "getLoadedMods", &ModManager::GetLoadedMods,
	    "getLoadedModsHash", &ModManager::GetLoadedModsHash,
	    "loadMods", &ModManager::LoadModsDeferred,
	    "unloadMods", &ModManager::UnloadModsDeferred);

	// TE_LB_USERTYPE(ScriptEngine);

	TE_LB_USERTYPE(
	    ScriptErrors,
	    "addLoadtimeError", &ScriptErrors::LuaAddLoadtimeError,
	    "addRuntimeError", &ScriptErrors::LuaAddRuntimeError,
	    "clearLoadtimeErrors", &ScriptErrors::ClearLoadtimeErrors,
	    "clearRuntimeErrors", &ScriptErrors::ClearRuntimeErrors,
	    "hasLoadtimeErrors", &ScriptErrors::HasLoadtimeError,
	    "hasRuntimeErrors", &ScriptErrors::HasRuntimeError);

	TE_LB_USERTYPE(
	    ScriptLoader,
	    "addReverseDependency", &ScriptLoader::LuaAddReverseDependency,
	    "getLoadingScriptID", &ScriptLoader::GetLoadingScriptID,
	    "getLoadingScriptName", &ScriptLoader::GetLoadingScriptName);

	TE_LB_USERTYPE(
	    ScriptProvider,
	    "getScriptIDByName", &ScriptProvider::GetScriptIDByName,
	    "getScriptNameByID", &ScriptProvider::GetScriptNameByID);

	auto TE = lua["TE"];
	TE["mods"] = &dynamic_cast<ModManager &>(context.GetModManager());
	TE["scriptEngine"] = &dynamic_cast<ScriptEngine &>(context.GetScriptEngine());
	TE["scriptErrors"] = &dynamic_cast<ScriptErrors &>(context.GetScriptErrors());
	TE["scriptLoader"] = &dynamic_cast<ScriptLoader &>(context.GetScriptLoader());
	TE["scriptProvider"] = &dynamic_cast<ScriptProvider &>(context.GetScriptProvider());

	lua.set_function("getModConfig", [this, &context](sol::string_view modUID) -> ModConfig *
	{
		std::shared_ptr<Mod> mod = context.GetModManager().FindLoadedMod(modUID);
		return mod == nullptr ? nullptr : &mod->GetConfig();
	});
}

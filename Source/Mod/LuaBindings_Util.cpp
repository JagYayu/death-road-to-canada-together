/**
 * @file Mod/LuaBindings_Util.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Mod/LuaBindings.hpp"

#include "Event/CoreEventsData.hpp"
#include "Util/MicrosImpl.hpp"
#include "Util/NoiseRandoms.hpp"
#include "Util/Version.hpp"

#include "sol/property.hpp"

using namespace tudov;

void LuaBindings::InstallUtil(sol::state &lua, Context &context) noexcept
{
	TE_LB_USERTYPE(
	    Version,
	    sol::call_constructor, sol::constructors<Version(), Version(Version::Type major, Version::Type minor, Version::Type patch)>(),
	    sol::meta_function::to_string, &Version::LuaToString,
	    "major", sol::property(&Version::LuaGetMajor, &Version::LuaSetMajor),
	    "minor", sol::property(&Version::LuaGetMinor, &Version::LuaSetMinor),
	    "patch", sol::property(&Version::LuaGetPatch, &Version::LuaSetPatch));

	TE_LB_USERTYPE(
	    PerlinNoiseRandom,
	    sol::call_constructor, sol::constructors<PerlinNoiseRandom(), PerlinNoiseRandom(std::int32_t seed), PerlinNoiseRandom(std::int32_t seed, std::float_t _frequency)>(),
	    "float1", &PerlinNoiseRandom::Float1,
	    "float2", &PerlinNoiseRandom::Float2,
	    "float3", &PerlinNoiseRandom::Float3,
	    "getFrequency", &PerlinNoiseRandom::GetFrequency,
	    "getSeed", &PerlinNoiseRandom::GetSeed,
	    "int1", &PerlinNoiseRandom::Int1,
	    "int2", &PerlinNoiseRandom::Int2,
	    "int3", &PerlinNoiseRandom::Int3,
	    "noise1", &PerlinNoiseRandom::Noise1,
	    "noise2", &PerlinNoiseRandom::Noise2,
	    "noise3", &PerlinNoiseRandom::Noise3,
	    "setFrequency", &PerlinNoiseRandom::SetFrequency,
	    "setSeed", &PerlinNoiseRandom::SetSeed);
}

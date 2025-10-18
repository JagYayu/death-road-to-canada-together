/**
 * @file Mod/LuaBindings_KeyModifier.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Mod/LuaBindings.hpp"

#include "System/KeyModifier.hpp"
#include "Util/MicrosImpl.hpp"

using namespace tudov;

void LuaBindings::InstallKeyModifier(sol::state &lua, Context &context) noexcept
{
	TE_LB_ENUM(
	    EKeyModifier,
	    {
	        {"None", EKeyModifier::None},
	        {"LeftShift", EKeyModifier::LeftShift},
	        {"RightShift", EKeyModifier::RightShift},
	        {"Level5", EKeyModifier::Level5},
	        {"LeftCtrl", EKeyModifier::LeftCtrl},
	        {"RightCtrl", EKeyModifier::RightCtrl},
	        {"LeftAlt", EKeyModifier::LeftAlt},
	        {"RightAlt", EKeyModifier::RightAlt},
	        {"LeftGui", EKeyModifier::LeftGui},
	        {"RightGui", EKeyModifier::RightGui},
	        {"Num", EKeyModifier::Num},
	        {"Caps", EKeyModifier::Caps},
	        {"Mode", EKeyModifier::Mode},
	        {"Scroll", EKeyModifier::Scroll},
	        {"Ctrl", EKeyModifier::Ctrl},
	        {"Shift", EKeyModifier::Shift},
	        {"Alt", EKeyModifier::Alt},
	        {"GUI", EKeyModifier::GUI},
	    });
}

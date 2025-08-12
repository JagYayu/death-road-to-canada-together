#include "debug/DebugScripts.hpp"

#include "mod/ScriptLoader.hpp"

using namespace tudov;

std::string_view DebugScripts::GetName() noexcept
{
	return Name();
}

void DebugScripts::UpdateAndRender(IWindow &window) noexcept
{
	IScriptLoader &scriptLoader = window.GetContext().GetScriptLoader();
	// scriptLoader.GetLoadErrors()

	// scriptLoader.GetOnLoadedScript().Invoke();
}

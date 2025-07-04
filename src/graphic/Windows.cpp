#include "Windows.h"
#include "graphic/Window.h"
#include <memory>

using namespace tudov;

Windows::Windows(Engine &engine) noexcept
    : engine(engine)
{
}

std::shared_ptr<Window> Windows::GetWindow(std::string_view id) noexcept
{
	for (auto &&entry : _windows)
	{
		if (std::get<0>(entry) == id)
		{
			return std::get<1>(entry);
		}
	}
	return nullptr;
}

std::shared_ptr<Window> Windows::CreateWindow(std::string_view id, std::string_view title, std::int32_t width, std::int32_t height)
{
	auto &&window = std::make_shared<Window>(engine);
	_windows.emplace_back(std::string(id), window);
	return window;
}

bool Windows::DestroyWindow(std::string_view id) noexcept
{
	for (auto it = _windows.begin(); it != _windows.end(); ++it)
	{
		if (std::get<0>(*it) == id)
		{
			_windows.erase(it);
			return true;
		}
	}
	return false;
}

void InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept
{
	
}

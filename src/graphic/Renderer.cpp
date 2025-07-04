#include "Renderer.h"

#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "graphic/DynamicBuffer.h"
#include "program/Engine.h"
#include <memory>
#include <string>

#undef DrawText

using namespace tudov;

Renderer::Renderer(Window &window) noexcept
    : window(window),
      _viewName()
{
}

void Renderer::Initialize() noexcept
{
	_viewName = std::string(View::CreateView());
}

void Renderer::Begin() noexcept
{
	if (auto &&viewWeak = View::GetView(_viewName); !viewWeak.expired())
	{
		auto view = viewWeak.lock();
		auto &&size = window.GetSize();

		view->SetRect(0, 0, std::get<0>(size), std::get<1>(size));
		view->SetClear();
		view->Touch();
	}
}

void Renderer::End() noexcept
{
}

glm::mat3x3 Renderer::GetTransform() noexcept
{
	return {};
}

void Renderer::SetTransform(const glm::mat3x3 &mat3) noexcept
{
	// bgfx::setViewTransform(_view->GetID(), );
}

void Renderer::Submit() noexcept
{
	if (auto &&viewWeak = View::GetView(_viewName); !viewWeak.expired())
	{
		bgfx::submit(viewWeak.lock()->GetID(), BGFX_INVALID_HANDLE);
	}
}

// void Renderer::Clear() noexcept
// {
// 	if (auto &&viewWeak = View::GetView(_viewName); !viewWeak.expired())
// 	{
// 		auto &&view = viewWeak.lock();

// 		auto &&id = view->GetID();

// 		const auto &size = window.GetSize();

// 		view->SetRect(0, 0, std::get<0>(size), std::get<1>(size));
// 		view->Touch();
// 	}
// }

sol::object Renderer::LuaGetTransform() noexcept
{
	return {};
}

void Renderer::LuaSetTransform(const sol::table &mat3) noexcept
{
}

std::shared_ptr<DynamicBuffer> Renderer::LuaNewDynamicBuffer() noexcept
{
	return std::make_shared<DynamicBuffer>(*this);
}

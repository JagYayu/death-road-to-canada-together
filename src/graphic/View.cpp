#include "View.h"
#include "bgfx/bgfx.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

using namespace tudov;

bgfx::ViewId View::_nextID = 0;
std::unordered_map<std::string, std::shared_ptr<View>> View::_viewMap = {};

std::weak_ptr<View> View::GetView(std::string_view name) noexcept
{
	return _viewMap.find(std::string(name))->second;
}

std::string_view View::CreateView(std::string_view name, SDL_Rect rectangle, uint32_t clearFlags, uint32_t color, float depth, unsigned char stencil)
{
	std::string key;
	if (name.empty())
	{
		key = std::to_string(_nextID);
	}
	else
	{
		key = std::string(name);
	}

	if (_viewMap.contains(key))
	{
		throw std::runtime_error("View already created");
	}

	return _viewMap.try_emplace(key, std::make_shared<View>(_nextID)).first->first;
}

bool View::RemoveView(std::string_view name) noexcept
{
	return _viewMap.erase(std::string(name));
}

View::View(bgfx::ViewId id) noexcept
    : _id(id)
{
}

View::~View() noexcept
{
	bgfx::resetView(_id);
}

void View::SetRect(std::uint16_t x, std::uint16_t y, std::uint16_t w, std::uint16_t h)
{
	bgfx::setViewRect(_id, x, y, w, h);
}

void View::SetClear(std::uint16_t flags, uint32_t color)
{
	bgfx::setViewClear(_id, flags, color, 1.0f, 0);
}

void View::Touch()
{
	bgfx::touch(_id);
}

void View::Reset()
{
}

void View::Submit(const bgfx::VertexBufferHandle &vbo, const bgfx::ProgramHandle &program, const float *transform)
{
}

bgfx::ViewId View::GetID() const noexcept
{
	return _id;
}

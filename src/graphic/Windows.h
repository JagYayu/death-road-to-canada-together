#pragma once

#include "Window.h"

#include <memory>
#include <tuple>
#include <vector>

#undef CreateWindow

namespace tudov
{
	class Windows
	{
	  private:
		std::vector<std::tuple<std::string, std::shared_ptr<Window>>> _windows;

	  private:
		class Iterator
		{
			using VecIter = decltype(_windows)::iterator;
			VecIter _it;

		  public:
			Iterator(VecIter it)
			    : _it(it)
			{
			}

			std::shared_ptr<Window> &operator*()
			{
				return std::get<1>(*_it);
			}
			Iterator &operator++()
			{
				++_it;
				return *this;
			}
			bool operator!=(const Iterator &other) const
			{
				return _it != other._it;
			}
		};

	  public:
		Engine &engine;

	  public:
		explicit Windows(Engine &engine) noexcept;

		std::shared_ptr<Window> GetWindow(std::string_view id) noexcept;
		std::shared_ptr<Window> CreateWindow(std::string_view id, std::string_view title, std::int32_t width, std::int32_t height);
		bool DestroyWindow(std::string_view id) noexcept;

		void InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept;

		Iterator begin()
		{
			return Iterator{_windows.begin()};
		}
		Iterator end()
		{
			return Iterator{_windows.end()};
		}
	};
} // namespace tudov

#pragma once

#include "DebugElement.hpp"
#include "DebugFileSystem.hpp"
#include "util/Scopes.hpp"

#include <memory>
#include <set>

namespace tudov
{
	struct IDebugManager
	{
		virtual ~IDebugManager() noexcept = default;

		virtual IDebugElement *GetElement(std::string_view elementName) noexcept = 0;
		virtual void AddElement(const std::shared_ptr<IDebugElement> &element) noexcept = 0;
		virtual bool RemoveElement(std::string_view elementName) noexcept = 0;
		virtual void UpdateAndRender(IWindow &window) noexcept = 0;

		inline const IDebugElement *GetElement(std::string_view elementName) const noexcept
		{
			return const_cast<IDebugManager *>(this)->GetElement(elementName);
		}

		template <ScopeIDebugElement TDebugElement>
		inline TDebugElement *GetElement() noexcept
		{
			auto &&element = GetElement(TDebugElement::Name());
			return element != nullptr ? dynamic_cast<TDebugElement *>(element) : nullptr;
		}

		template <ScopeIDebugElement TDebugElement>
		inline const TDebugElement *GetElement() const noexcept
		{
			return const_cast<IDebugManager *>(this)->GetElement<TDebugElement>();
		}
	};

	struct IWindow;
	class DebugConsole;
	class DebugLog;
	class DebugProfiler;
	class DebugScripts;

	class DebugManager : public IDebugManager
	{
	  protected:
		std::vector<std::shared_ptr<IDebugElement>> _elements;
		std::set<std::string_view> _shownElements;

	  public:
		std::shared_ptr<DebugConsole> console;
		std::shared_ptr<DebugFileSystem> fileSystem;
		std::shared_ptr<DebugLog> log;
		std::shared_ptr<DebugProfiler> profiler;
		std::shared_ptr<DebugScripts> scripts;

	  public:
		explicit DebugManager() noexcept;
		~DebugManager() noexcept override = default;

		IDebugElement *GetElement(std::string_view elementName) noexcept override;
		void AddElement(const std::shared_ptr<IDebugElement> &element) noexcept override;
		bool RemoveElement(std::string_view elementName) noexcept override;
		void UpdateAndRender(IWindow &window) noexcept override;
	};
} // namespace tudov

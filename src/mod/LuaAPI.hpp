#pragma once

#include "ScriptEngine.hpp"

#include "sol/state.hpp"

#include <functional>
#include <vector>

namespace tudov
{
	class Engine;

	struct ILuaAPI
	{
		using TInstallation = std::function<void(sol::state &)>;

		virtual ~ILuaAPI() noexcept = default;

		virtual void RegisterInstallation(const TInstallation &installation) noexcept = 0;
		virtual void Install(sol::state &lua, Context &context) noexcept = 0;
	};

	class LuaAPI : public ILuaAPI
	{
	  private:
		std::vector<TInstallation> _installations;

	  public:
		explicit LuaAPI() noexcept = default;

		void RegisterInstallation(const TInstallation &installation) noexcept override;
		void Install(sol::state &lua, Context &context) noexcept override;
	};
} // namespace tudov

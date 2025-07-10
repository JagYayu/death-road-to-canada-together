#pragma once

#include "ScriptEngine.hpp"

#include "sol/state.hpp"
#include "util/Log.hpp"

#include <functional>
#include <string_view>
#include <tuple>
#include <vector>

namespace tudov
{
	class Engine;

	struct ILuaAPI
	{
		using TInstallation = std::function<void(sol::state &)>;

		virtual ~ILuaAPI() noexcept = default;

		virtual bool RegisterInstallation(std::string_view key, const TInstallation &installation) = 0;
		virtual void Install(sol::state &lua, Context &context) = 0;
	};

	class LuaAPI : public ILuaAPI
	{
	  private:
		std::vector<std::tuple<std::string, TInstallation>> _installations;

	  public:
		explicit LuaAPI() noexcept = default;

		bool RegisterInstallation(std::string_view key, const TInstallation &installation) override;
		void Install(sol::state &lua, Context &context) override;
	};
} // namespace tudov

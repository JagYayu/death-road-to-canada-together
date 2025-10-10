/**
 * @file mod/ScriptErrors.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Program/EngineComponent.hpp"
#include "sol/forward.hpp"
#include "util/Definitions.hpp"
#include "util/Micros.hpp"

#include <chrono>
#include <cstdint>
#include <deque>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace tudov
{
	class LuaBindings;

	struct ScriptError final
	{
		std::chrono::time_point<std::chrono::system_clock> time;
		ScriptID scriptID;
		std::uint32_t line;
		std::string message;

		static std::uint32_t ExtractLuaErrorLine(std::string_view message);

		explicit ScriptError(ScriptID scriptID, std::string_view message) noexcept;
		~ScriptError() noexcept = default;
	};

	struct IScriptErrors : public IEngineComponent
	{
		virtual bool HasLoadtimeError() const noexcept = 0;
		virtual bool HasRuntimeError() const noexcept = 0;

		virtual std::vector<std::shared_ptr<ScriptError>> GetLoadtimeErrors() const noexcept = 0;
		virtual std::vector<std::shared_ptr<ScriptError>> GetRuntimeErrors() const noexcept = 0;

		virtual const std::vector<std::shared_ptr<ScriptError>> &GetLoadtimeErrorsCached() const noexcept = 0;
		virtual const std::vector<std::shared_ptr<ScriptError>> &GetRuntimeErrorsCached() const noexcept = 0;

		virtual void ClearCaches() noexcept = 0;

		virtual void AddLoadtimeError(const std::shared_ptr<ScriptError> &scriptError) = 0;
		virtual void AddRuntimeError(const std::shared_ptr<ScriptError> &scriptError) = 0;

		virtual bool RemoveLoadtimeError(ScriptID scriptID) = 0;
		virtual bool RemoveRuntimeError(ScriptID scriptID) = 0;

		virtual void ClearLoadtimeErrors() noexcept = 0;
		virtual void ClearRuntimeErrors() noexcept = 0;

		template <typename... TArgs>
		    requires(!(std::same_as<std::shared_ptr<ScriptError>, std::decay_t<TArgs>> || ...))
		TE_FORCEINLINE void AddLoadtimeError(TArgs &&...args)
		{
			AddLoadtimeError(std::make_shared<ScriptError>(std::forward<TArgs>(args)...));
		}
		template <typename... TArgs>
		    requires(!(std::same_as<std::shared_ptr<ScriptError>, std::decay_t<TArgs>> || ...))
		TE_FORCEINLINE void AddRuntimeError(TArgs &&...args)
		{
			AddRuntimeError(std::make_shared<ScriptError>(std::forward<TArgs>(args)...));
		}
	};

	class ScriptErrors final : public IScriptErrors
	{
		friend LuaBindings;

	  protected:
		Context &_context;
		std::unordered_map<ScriptID, std::shared_ptr<ScriptError>> _scriptLoadtimeErrors;
		std::deque<std::shared_ptr<ScriptError>> _scriptRuntimeErrors;

		DelegateEventHandlerID _handlerIDOnPreLoadAllScripts = 0;
		DelegateEventHandlerID _handlerIDOnUnloadScript = 0;
		DelegateEventHandlerID _handlerIDOnFailedLoadScript = 0;

		mutable std::optional<std::vector<std::shared_ptr<ScriptError>>> _scriptLoadtimeErrorCache;
		mutable std::optional<std::vector<std::shared_ptr<ScriptError>>> _scriptRuntimeErrorsCached;

	  public:
		explicit ScriptErrors(Context &context) noexcept;
		explicit ScriptErrors(const ScriptErrors &) noexcept = delete;
		explicit ScriptErrors(ScriptErrors &&) noexcept = delete;
		ScriptErrors &operator=(const ScriptErrors &) noexcept = delete;
		ScriptErrors &operator=(ScriptErrors &&) noexcept = delete;
		~ScriptErrors() noexcept override = default;

		Context &GetContext() noexcept override;

		void PreInitialize() noexcept override;
		void PostDeinitialize() noexcept override;

		bool HasLoadtimeError() const noexcept override;
		bool HasRuntimeError() const noexcept override;

		std::vector<std::shared_ptr<ScriptError>> GetLoadtimeErrors() const noexcept override;
		std::vector<std::shared_ptr<ScriptError>> GetRuntimeErrors() const noexcept override;

		const std::vector<std::shared_ptr<ScriptError>> &GetLoadtimeErrorsCached() const noexcept override;
		const std::vector<std::shared_ptr<ScriptError>> &GetRuntimeErrorsCached() const noexcept override;

		void ClearCaches() noexcept override;

		void AddLoadtimeError(const std::shared_ptr<ScriptError> &scriptError) override;
		void AddRuntimeError(const std::shared_ptr<ScriptError> &scriptError) override;

		bool RemoveLoadtimeError(ScriptID scriptID) override;
		bool RemoveRuntimeError(ScriptID scriptID) override;

		void ClearLoadtimeErrors() noexcept override;
		void ClearRuntimeErrors() noexcept override;

	  private:
		void LuaAddLoadtimeError(sol::object scriptID, sol::object traceback) noexcept;
		void LuaAddRuntimeError(sol::object scriptID, sol::object traceback) noexcept;
		bool LuaRemoveLoadtimeError(sol::object scriptID) noexcept;
		bool LuaRemoveRuntimeError(sol::object scriptID) noexcept;
	};
} // namespace tudov

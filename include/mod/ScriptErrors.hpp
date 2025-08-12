#pragma once

#include "program/EngineComponent.hpp"
#include "util/Definitions.hpp"

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace tudov
{
	enum class EScriptErrorType : std::uint8_t
	{
		Loadtime = 0,
		Runtime,
	};

	struct ScriptError
	{
		std::chrono::time_point<std::chrono::system_clock> time;
		ScriptID scriptID;
		std::uint32_t line;
		std::string message;

		static std::uint32_t ExtractLuaErrorLine(std::string_view message);

		explicit ScriptError(ScriptID scriptID, EScriptErrorType type, std::string_view message) noexcept;
	};

	struct IScriptErrors : public IEngineComponent
	{
		virtual bool HasLoadtimeError() const noexcept = 0;
		virtual bool HasRuntimeError() const noexcept = 0;

		virtual std::vector<std::shared_ptr<ScriptError>> GetLoadtimeErrors() const noexcept = 0;
		virtual std::vector<std::shared_ptr<ScriptError>> GetRuntimeErrors() const noexcept = 0;

		virtual const std::vector<std::shared_ptr<ScriptError>> &GetLoadtimeErrorsCached() noexcept = 0;
		virtual const std::vector<std::shared_ptr<ScriptError>> &GetRuntimeErrorsCached() noexcept = 0;

		virtual void ClearCaches() noexcept = 0;

		virtual void AddLoadtimeError(const std::shared_ptr<ScriptError> &scriptError) = 0;
		virtual void AddRuntimeError(const std::shared_ptr<ScriptError> &scriptError) = 0;

		virtual bool RemoveLoadtimeError(ScriptID scriptID) = 0;
		virtual bool RemoveRuntimeError(ScriptID scriptID) = 0;

		virtual void ClearLoadtimeErrors() noexcept = 0;
		virtual void ClearRuntimeErrors() noexcept = 0;
	};

	class ScriptErrors : public IScriptErrors
	{
	  protected:
		Context &_context;
		std::unordered_map<ScriptID, std::shared_ptr<ScriptError>> _scriptLoadtimeErrors;
		// std::unordered_map<ScriptID, std::shared_ptr<ScriptError>> _scriptLoadErrorsCache;
		std::optional<std::vector<std::shared_ptr<ScriptError>>> _scriptLoadtimeErrorCached;
		std::vector<std::shared_ptr<ScriptError>> _scriptRuntimeErrors;

	  public:
		explicit ScriptErrors(Context &context) noexcept;
		~ScriptErrors() noexcept override = default;

		Context &GetContext() noexcept override;

		bool HasLoadtimeError() const noexcept override;
		bool HasRuntimeError() const noexcept override;

		std::vector<std::shared_ptr<ScriptError>> GetLoadtimeErrors() const noexcept override;
		std::vector<std::shared_ptr<ScriptError>> GetRuntimeErrors() const noexcept override;

		const std::vector<std::shared_ptr<ScriptError>> &GetLoadtimeErrorsCached() noexcept override;
		const std::vector<std::shared_ptr<ScriptError>> &GetRuntimeErrorsCached() noexcept override;

		void ClearCaches() noexcept override;

		void AddLoadtimeError(const std::shared_ptr<ScriptError> &scriptError) override;
		void AddRuntimeError(const std::shared_ptr<ScriptError> &scriptError) override;

		bool RemoveLoadtimeError(ScriptID scriptID) override;
		bool RemoveRuntimeError(ScriptID scriptID) override;

		void ClearLoadtimeErrors() noexcept override;
		void ClearRuntimeErrors() noexcept override;
	};
} // namespace tudov

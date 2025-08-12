#pragma once

#include "util/Definitions.hpp"

#include <chrono>
#include <cstdint>
#include <string>

namespace tudov
{
	enum class EScriptErrorType : std::uint8_t
	{
		Loadtime = 0,
		Runtime,
	};

	struct ScriptError
	{
		EScriptErrorType type;
		std::chrono::time_point<std::chrono::system_clock> time;
		ScriptID scriptID;
		std::uint32_t line;
		std::string message;

		static std::uint32_t ExtractLuaErrorLine(std::string_view message);

		explicit ScriptError(ScriptID scriptID, EScriptErrorType type, std::string_view message) noexcept;
	};
} // namespace tudov

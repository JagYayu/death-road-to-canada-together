#include "mod/ScriptError.hpp"
#include <regex>

using namespace tudov;

std::uint32_t ScriptError::ExtractLuaErrorLine(std::string_view error)
{
	std::regex pattern{R"((?:\[string "[^"]*"\]|.+?):(\d+):)"};
	const char *begin = error.data();
	const char *end = begin + error.size();

	std::cregex_iterator it(begin, end, pattern);
	std::cregex_iterator endIt;

	if (it != endIt)
	{
		const std::cmatch &match = *it;
		if (match.size() >= 2)
		{
			try
			{
				return static_cast<std::uint32_t>(std::stoi(match[1].str()));
			}
			catch (...)
			{
				return 0;
			}
		}
	}

	return 0;
}

ScriptError::ScriptError(ScriptID scriptID, EScriptErrorType type, std::string_view message) noexcept
    : type(EScriptErrorType::Loadtime),
      time(std::chrono::system_clock::now()),
      scriptID(scriptID),
      line(ScriptError::ExtractLuaErrorLine(message)),
      message(std::string(message))
{
}

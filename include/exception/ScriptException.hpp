/**
 * @file exception/ScriptException.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "exception/Exception.hpp"
#include "program/Context.hpp"
#include "util/Definitions.hpp"

namespace tudov
{
	class ScriptException : public Exception
	{
	  public:
		ScriptID scriptID;
		std::string traceback;

		explicit ScriptException(Context &context, ScriptID scriptID, std::string traceback) noexcept;
		~ScriptException() noexcept override = default;

		std::string_view What() const noexcept override;
	};
} // namespace tudov

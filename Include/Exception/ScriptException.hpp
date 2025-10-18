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

#include "Exception/Exception.hpp"
#include "Program/Context.hpp"
#include "Util/Definitions.hpp"

namespace tudov
{
	class ScriptException : public Exception
	{
	  public:
		ScriptID scriptID;
		std::string traceback;

		explicit ScriptException(Context &context, ScriptID scriptID, std::string traceback) noexcept;
		explicit ScriptException(const ScriptException &) noexcept = default;
		explicit ScriptException(ScriptException &&) noexcept = default;
		ScriptException &operator=(const ScriptException &) noexcept = delete;
		ScriptException &operator=(ScriptException &&) noexcept = delete;
		~ScriptException() noexcept override = default;

		std::string_view What() const noexcept override;
	};
} // namespace tudov

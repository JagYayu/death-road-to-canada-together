/**
 * @file mod/ScriptProcessor.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Util/Micros.hpp"

#include <stdexcept>
#include <string>

namespace tudov
{
	enum class EScriptProcessor
	{
		Obfuscate = 1 << 0,
		LJEncode = 1 << 1,
		Compress = 1 << 2,

		None = 0,
		ClosedSource = Obfuscate | LJEncode | Compress,
	};

	struct ScriptProcessor
	{
		TE_STATIC_CLASS(ScriptProcessor);

		[[noreturn]]
		static std::string Serialize(EScriptProcessor options)
		{
			throw std::runtime_error("NOT IMPLEMENT YET");
		}

		[[noreturn]]
		static std::string Deserialize(EScriptProcessor options)
		{
			throw std::runtime_error("NOT IMPLEMENT YET");
		}
	};
} // namespace tudov

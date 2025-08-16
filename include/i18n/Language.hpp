/**
 * @file i18n/Language.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "json.hpp"

#include <string_view>
#include <variant>

namespace tudov
{
	enum class ELanguage
	{
		ChineseSimplified,
		ChineseTraditional,
		English,
		French,
		German,
		Japanese,
		Korean,
		Russian,
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(ELanguage, {
	                                            {ELanguage::ChineseSimplified, "ChineseSimplified"},
	                                            {ELanguage::ChineseTraditional, "ChineseTraditional"},
	                                            {ELanguage::English, "English"},
	                                            {ELanguage::French, "French"},
	                                            {ELanguage::German, "German"},
	                                            {ELanguage::Japanese, "Japanese"},
	                                            {ELanguage::Korean, "Korean"},
	                                            {ELanguage::Russian, "Russian"},
	                                        });

	using Language = std::variant<ELanguage, std::string_view>;
} // namespace tudov

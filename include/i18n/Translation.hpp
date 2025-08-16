/**
 * @file i18n/Translation.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Language.hpp"
#include "resource/Resource.hpp"

#include <unordered_map>
#include <vector>

namespace tudov
{
	class Translation : public IResource
	{
	  private:
		std::string_view _path;
		std::vector<Language> _languages;
		std::vector<std::string> _textKeys;
		std::unordered_map<Language, std::unordered_map<std::string_view, std::string>> _texts;

	  public:
		explicit Translation(std::string_view path) noexcept;
		~Translation() noexcept;

	  public:
		std::string_view GetFilePath() const noexcept override;

		bool HasLanguage(Language lang) const noexcept;
		std::vector<Language> GetLanguages() const noexcept;
		bool HasTextKey(std::string_view key) const noexcept;
		std::vector<std::string> GetTextKeys() const noexcept;
		std::string_view GetText(Language lang, std::string_view key) const noexcept;
	};
} // namespace tudov

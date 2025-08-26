/**
 * @file i18n/Localization.hpp
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
#include "TranslationPack.hpp"
#include "sol/forward.hpp"
#include "sol/string_view.hpp"
#include "system/Log.hpp"

#include <format>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace tudov
{
	class Translation;

	struct ILocalization
	{
		virtual ~ILocalization() noexcept = default;

		virtual ELanguage GetDefaultLanguage() const noexcept;

		virtual Language GetFirstLanguage() const noexcept = 0;
		virtual std::vector<Language> GetLanguageOrders() const noexcept = 0;
		virtual std::vector<std::weak_ptr<TranslationPack>> GetTranslationPacks() const noexcept = 0;
		virtual std::string_view GetText(std::string_view key) const noexcept = 0;

		template <typename... TArgs>
		std::string GetFormattedText(std::string_view key, TArgs &&...args) const noexcept
		{
			auto format = GetText(key);
			try
			{
				return std::vformat(format, std::make_format_args(std::forward<TArgs>(args)...));
			}
			catch (...)
			{
				return std::string(format);
			}
		}
	};

	class Localization : public ILocalization, private ILogProvider
	{
	  private:
		std::shared_ptr<Log> _log;
		std::vector<Language> _languageOrders;
		std::vector<std::shared_ptr<TranslationPack>> _translationPacks;

		mutable std::unordered_map<std::string, std::string_view> _caches;

	  public:
		explicit Localization() noexcept;
		~Localization() noexcept override = default;

		Log &GetLog() noexcept override;

		Language GetFirstLanguage() const noexcept override;
		std::vector<Language> GetLanguageOrders() const noexcept override;
		std::vector<std::weak_ptr<TranslationPack>> GetTranslationPacks() const noexcept override;
		bool HasTextKey(std::string_view key) const noexcept;
		std::string_view GetText(std::string_view key) const noexcept override;

	  private:
		std::string_view GetTextUncached(std::string_view key) const noexcept;

	  private:
		sol::string_view LuaGetText(sol::object key) const noexcept;
	};
} // namespace tudov

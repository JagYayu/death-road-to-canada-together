/**
 * @file i18n/Localization.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "I18n/Localization.hpp"

#include "I18n/BuiltinTexts.hpp"
#include "I18n/Language.hpp"
#include "I18n/TranslationPack.hpp"
#include "Util/Utils.hpp"
#include <string_view>

using namespace tudov;

ELanguage ILocalization::GetDefaultLanguage() const noexcept
{
	return ELanguage::English;
}

Localization::Localization() noexcept
    : _log(Log::Get("Localization"))
{
}

Log &Localization::GetLog() noexcept
{
	return *_log;
}

Language Localization::GetFirstLanguage() const noexcept
{
	if (_languageOrders.empty())
	{
		return GetDefaultLanguage();
	}

	return _languageOrders[0];
}

std::vector<Language> Localization::GetLanguageOrders() const noexcept
{
	return _languageOrders;
}

std::vector<std::weak_ptr<TranslationPack>> Localization::GetTranslationPacks() const noexcept
{
	return FromSharedToWeak<TranslationPack>(_translationPacks);
}

std::string_view Localization::GetText(std::string_view key) const noexcept
{
	auto it = _caches.find(key.data());
	if (it != _caches.end()) [[likely]]
	{
		return it->second;
	}

	std::string_view text = GetTextUncached(key);
	_caches[std::string(key)] = text;
	return text;
}

std::string_view Localization::GetTextUncached(std::string_view key) const noexcept
{
	for (const auto &pack : _translationPacks)
	{
		if (pack->translation.HasTextKey(key))
		{
			for (const auto &lang : _languageOrders)
			{
				if (pack->translation.HasLanguage(lang))
				{
					return pack->translation.GetText(lang, key);
				}
			}
		}
	}

	return BuiltinTexts::GetFallbackText(key);
}

sol::string_view Localization::LuaGetText(sol::object key) const noexcept
{
	auto key_ = key.as<std::string_view>();

	return GetText(key_);
}

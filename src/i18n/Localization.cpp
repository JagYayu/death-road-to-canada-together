#include "i18n/Localization.hpp"
#include "i18n/Language.hpp"
#include "i18n/Translation.hpp"
#include "util/Utils.hpp"

#include <cassert>
#include <cstddef>

using namespace tudov;

Localization::Localization() noexcept
    : _log(Log::Get("Localization"))
{
}

Language ILocalization::GetDefaultLanguage() noexcept
{
	return ELanguage::English;
}

Language Localization::GetLanguage() noexcept
{
	return _language;
}

std::vector<std::weak_ptr<Translation>> Localization::GetTranslations() noexcept
{
	return FromSharedToWeak<Translation>(_translations);
}

std::optional<std::string_view> Localization::GetText(std::string_view key, std::variant<Language, std::nullptr_t> lang) noexcept
{
	const Language &language = std::holds_alternative<std::nullptr_t>(lang) ? _language : std::get<Language>(lang);

	auto &&it = _language2translationsCaches.find(language);
	if (it == _language2translationsCaches.end()) [[unlikely]]
	{
		it = _language2translationsCaches.try_emplace(language, std::vector<std::weak_ptr<Translation>>()).first;

		for (auto &&translation : _translations)
		{
			if (translation->GetLanguage() == language)
			{
				it->second.emplace_back(translation);
			}
		}
	}

	for (auto &&translation : it->second)
	{
		if (translation.expired()) [[unlikely]]
		{
			_log->Warn("Got a expired weak ptr of translation, did you forgot to clear cache?");
			continue;
		}

		if (auto &&text = translation.lock()->GetText(key); !text->empty())
		{
			return *text;
		}
	}
	return std::nullopt;
}

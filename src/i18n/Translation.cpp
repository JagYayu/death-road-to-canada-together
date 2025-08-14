#include "i18n/Translation.hpp"
#include "sol/string_view.hpp"

#include <optional>
#include <vector>

using namespace tudov;

// bool Translation::operator>(const ITranslation &other) const noexcept
// {
// 	try
// 	{
// 		auto &&translation = dynamic_cast<const Translation &>(other);
// 		if (priority != translation.priority)
// 		{
// 			return priority > translation.priority;
// 		}
// 		if (index != translation.index)
// 		{
// 			return index < translation.index;
// 		}
// 	}
// 	catch (std::bad_cast &e)
// 	{
// 	}
// 	return (void *)this > (void *)&other;
// }

// bool Translation::operator==(const ITranslation &other) const noexcept
// {
// 	try
// 	{
// 		auto &&translation = dynamic_cast<const Translation &>(other);
// 		return priority == translation.priority && index == translation.index;
// 	}
// 	catch (std::bad_cast &e)
// 	{
// 		return (void *)this == (void *)&other;
// 	}
// }

Translation::Translation(sol::string_view path) noexcept
    : _path(path)
{
}

Translation::~Translation() noexcept
{
}

std::string_view Translation::GetFilePath() const noexcept
{
	return _path;
}

bool Translation::HasLanguage(Language lang) const noexcept
{
	for (auto &language : _languages)
	{
		if (language == lang)
		{
			return true;
		}
	}

	return false;
}

std::vector<Language> Translation::GetLanguages() const noexcept
{
	return _languages;
}

bool Translation::HasTextKey(std::string_view key) const noexcept
{
	for (auto &textKey : _textKeys)
	{
		if (textKey == key)
		{
			return true;
		}
	}

	return false;
}

std::string_view Translation::GetText(Language lang, std::string_view key) const noexcept
{
	auto it = _texts.find(lang);
	if (it == _texts.end())
	{
		return "";
	}

	const std::unordered_map<std::string_view, std::string> &texts = it->second;
	auto it1 = texts.find(key);
	if (it1 == texts.end())
	{
		return "";
	}

	return it1->second;
}

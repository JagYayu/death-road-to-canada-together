#include "i18n/Translation.hpp"

#include <optional>

using namespace tudov;

bool Translation::operator>(const ITranslation &other) const noexcept
{
	try
	{
		auto &&translation = dynamic_cast<const Translation &>(other);
		if (priority != translation.priority)
		{
			return priority > translation.priority;
		}
		if (index != translation.index)
		{
			return index < translation.index;
		}
	}
	catch (std::bad_cast &e)
	{
	}
	return (void *)this > (void *)&other;
}

bool Translation::operator==(const ITranslation &other) const noexcept
{
	try
	{
		auto &&translation = dynamic_cast<const Translation &>(other);
		return priority == translation.priority && index == translation.index;
	}
	catch (std::bad_cast &e)
	{
		return (void *)this == (void *)&other;
	}
}

Language Translation::GetLanguage() noexcept
{
	return _language;
}

std::optional<std::string_view> Translation::GetText(std::string_view key) const noexcept
{
	auto &&it = _textPool.find(key);
	return it != _textPool.end() ? std::make_optional(std::string_view(it->second)) : std::nullopt;
}

void Translation::SetText(std::string_view key, std::string_view value) noexcept
{
	_textPool[std::string(key)] = std::string(value);
}

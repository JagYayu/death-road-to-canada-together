#pragma once

#include "Language.hpp"
#include "program/EngineComponent.hpp"
#include "util/Log.hpp"

#include <cstddef>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

namespace tudov
{
	class Translation;

	struct ILocalization : public IEngineComponent
	{
		virtual Language GetLanguage() noexcept = 0;
		virtual std::vector<std::weak_ptr<Translation>> GetTranslations() noexcept = 0;
		virtual std::optional<std::string_view> GetText(std::string_view key, std::variant<Language, std::nullptr_t> lang) noexcept = 0;

		virtual Language GetDefaultLanguage() noexcept;
	};

	class Localization : public ILocalization, public ILogProvider
	{
	  private:
		std::shared_ptr<Log> _log;
		Language _language;
		std::vector<std::shared_ptr<Translation>> _translations;
		std::unordered_map<Language, std::vector<std::weak_ptr<Translation>>> _language2translationsCaches;

	  public:
		explicit Localization() noexcept;
		virtual ~Localization() noexcept = default;

		Language GetLanguage() noexcept override;
		std::vector<std::weak_ptr<Translation>> GetTranslations() noexcept override;
		std::optional<std::string_view> GetText(std::string_view key, std::variant<Language, std::nullptr_t> lang) noexcept override;
	};
} // namespace tudov

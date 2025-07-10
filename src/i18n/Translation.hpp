#pragma once

#include "Language.hpp"
#include "util/StringUtils.hpp"
#include <unordered_map>

namespace tudov
{
	struct ITranslation
	{
		virtual bool operator>(const ITranslation &other) const noexcept = 0;
		virtual bool operator==(const ITranslation &other) const noexcept = 0;

		virtual Language GetLanguage() noexcept = 0;
		virtual std::optional<std::string_view> GetText(std::string_view key) const noexcept = 0;
		virtual void SetText(std::string_view key, std::string_view value) noexcept = 0;

		inline bool operator<(const ITranslation &other) const noexcept
		{
			return !(*this > other) && !(*this == other);
		}

		inline bool operator>=(const ITranslation &other) const noexcept
		{
			return *this > other || *this == other;
		}

		inline bool operator<=(const ITranslation &other) const noexcept
		{
			return !(*this > other);
		}

		inline bool operator!=(const ITranslation &other) const noexcept
		{
			return !(*this == other);
		}
	};

	class Translation : public ITranslation
	{
	  private:
		std::int32_t index;
		std::int32_t priority;
		Language _language;
		std::unordered_map<std::string, std::string, StringSVHash, StringSVEqual> _textPool;

	  public:
		bool operator>(const ITranslation &translation) const noexcept override;
		bool operator==(const ITranslation &translation) const noexcept override;

		Language GetLanguage() noexcept override;
		std::optional<std::string_view> GetText(std::string_view key) const noexcept override;
		void SetText(std::string_view key, std::string_view value) noexcept override;
	};
} // namespace tudov

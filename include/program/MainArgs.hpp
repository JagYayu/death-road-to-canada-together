/**
 * @file program/MainArgs.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace tudov
{
	class MainArgs
	{
	  private:
		std::vector<std::string> _args;
		std::unordered_map<std::string, std::string> _values;

	  public:
		explicit MainArgs() noexcept = default;

		explicit MainArgs(int argc, char **argv) noexcept
		{
			for (int i = 0; i < argc; ++i)
			{
				_args.emplace_back(argv[i]);
			}

			for (int i = 1; i < argc; ++i)
			{
				std::string arg = argv[i];

				if (arg.rfind("--", 0) == 0 || arg.rfind("-", 0) == 0)
				{
					size_t prefixLen = arg[1] == '-' ? 2 : 1;
					std::string keyval = arg.substr(prefixLen);

					size_t eqPos = keyval.find('=');
					if (eqPos != std::string::npos)
					{
						std::string key = keyval.substr(0, eqPos);
						std::string value = keyval.substr(eqPos + 1);
						_values[key] = value;
					}
					else
					{
						_values[keyval] = "";
					}
				}
			}
		}

		inline size_t Size() const
		{
			return _args.size();
		}

		inline std::vector<std::string> GetRaw() noexcept
		{
			return _args;
		}

		inline const std::vector<std::string> GetRaw() const noexcept
		{
			return _args;
		}

		inline const std::string &operator[](size_t i) const
		{
			return _args[i];
		}

		inline std::string_view operator[](std::string_view key) const
		{
			return _values.at(key.data());
		}

		inline std::optional<std::string_view> GetValue(std::string_view key) const noexcept
		{
			if (auto &&it = _values.find(key.data()); it != _values.end())
			{
				return it->second;
			}
			else
			{
				return std::nullopt;
			}
		}

		inline auto begin() const
		{
			return _args.begin();
		}

		inline auto end() const
		{
			return _args.end();
		}
	};
} // namespace tudov

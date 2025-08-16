/**
 * @file util/IndexedSet.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <unordered_map>
#include <vector>

namespace tudov
{
	template <typename T, typename TVector = std::vector<T>, typename TMap = std::unordered_map<T, std::size_t>>
	class IndexedSet
	{
	  private:
		TVector values;
		TMap indexMap;

	  public:
		std::size_t Add(const T &value)
		{
			auto it = indexMap.find(value);
			if (it != indexMap.end())
			{
				return it->second;
			}

			std::size_t index = values.size();
			values.push_back(value);
			indexMap[value] = index;
			return index;
		}

		const T &operator[](std::size_t index) const
		{
			return values[index];
		}
		std::size_t operator[](T value) const
		{
			return indexMap[value];
		}

		std::size_t size() const
		{
			return values.size();
		}

		auto begin() const
		{
			return values.begin();
		}
		auto end() const
		{
			return values.end();
		}
	};
} // namespace tudov

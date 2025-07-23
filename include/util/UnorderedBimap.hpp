#pragma once

#include <unordered_map>

namespace tudov
{
	template <typename TKey, typename TValue,
	          typename TKeyHash = std::hash<TKey>,
	          typename TValueHash = std::hash<TValue>,
	          typename TKeyEqual = std::equal_to<TKey>,
	          typename TValueEqual = std::equal_to<TValue>,
	          typename TKeyAlloc = std::allocator<std::pair<const TKey, TValue>>,
	          typename TValueAlloc = std::allocator<std::pair<const TValue, TKey>>,
	          typename TKeyUnorderedMap = std::unordered_map<TKey, TValue, TKeyHash, TKeyEqual, TKeyAlloc>,
	          typename TValueUnorderedMap = std::unordered_map<TValue, TKey, TValueHash, TValueEqual, TValueAlloc>>
	class UnorderedBimap
	{
	  private:
		TKeyUnorderedMap _key2value;
		TValueUnorderedMap _value2key;

	  public:
		inline size_t Size() const
		{
			return _key2value.size();
		}

		inline bool Empty() const
		{
			return _key2value.empty();
		}

		inline void Clear()
		{
			_key2value.clear();
			_value2key.clear();
		}

		inline bool Insert(const TKey &key, const TValue &value)
		{
			if (_key2value.count(key) || _value2key.count(value))
			{
				return false;
			}

			_key2value[key] = value;
			_value2key[value] = key;
			return true;
		}

		inline TValue &AtKey(const TKey &key)
		{
			return _key2value.at(key);
		}

		inline const TValue &AtKey(const TKey &key) const
		{
			return const_cast<UnorderedBimap *>(this)->AtKey(key);
		}

		inline TKey &AtValue(const TValue &value)
		{
			return _value2key.at(value);
		}

		inline const TKey &AtValue(const TValue &value) const
		{
			return const_cast<UnorderedBimap *>(this)->AtValue(value);
		}

		inline TValue *FindByKey(const TKey &key)
		{
			auto it = _key2value.find(key);
			return it != _key2value.end() ? &it->second : nullptr;
		}

		const TValue *FindByKey(const TKey &key) const
		{
			return const_cast<UnorderedBimap *>(this)->FindByKey(key);
		}

		inline TKey *FindByValue(const TValue &value)
		{
			auto it = _value2key.find(value);
			return it != _value2key.end() ? &it->second : nullptr;
		}

		const TKey *FindByValue(const TValue &value) const
		{
			return const_cast<UnorderedBimap *>(this)->FindByValue(value);
		}

		inline bool EraseByKey(const TKey &key)
		{
			auto it = _key2value.find(key);
			if (it == _key2value.end())
			{
				return false;
			}

			_value2key.erase(it->second);
			_key2value.erase(it);
			return true;
		}

		inline bool EraseByValue(const TValue &value)
		{
			auto it = _value2key.find(value);
			if (it == _value2key.end())
			{
				return false;
			}

			_key2value.erase(it->second);
			_value2key.erase(it);
			return true;
		}

		inline TKeyUnorderedMap &GetKey2Value()
		{
			return _key2value;
		}

		inline const TKeyUnorderedMap &GetKey2Value() const
		{
			return const_cast<UnorderedBimap *>(this)->GetKey2Value();
		}

		inline TValueUnorderedMap &GetValue2Key()
		{
			return _value2key;
		}

		inline const TValueUnorderedMap &GetValue2Key() const
		{
			return const_cast<UnorderedBimap *>(this)->GetValue2Key();
		}

		inline auto Begin()
		{
			return _key2value.begin();
		}

		inline auto End()
		{
			return _key2value.end();
		}

		inline auto begin()
		{
			return Begin();
		}
		inline auto end()
		{
			return End();
		}
	};
} // namespace tudov

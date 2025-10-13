/**
 * @file util/UnorderedBimap.hpp
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
		class ValueProxy
		{
		  private:
			UnorderedBimap &_parent;
			const TKey &_key;

		  public:
			explicit ValueProxy(UnorderedBimap &parent, const TKey &key) noexcept
			    : _parent(parent),
			      _key(key)
			{
			}

			operator TValue &()
			{
				return _parent._key2value[_key];
			}

			operator const TValue &() const
			{
				return _parent._key2value[_key];
			}

			ValueProxy &operator=(const TValue &value)
			{
				_parent.EraseByKey(_key);
				_parent.Insert(_key, value);
				return *this;
			}
		};

		class KeyProxy
		{
		  private:
			UnorderedBimap &_parent;
			const TValue &_value;

		  public:
			explicit KeyProxy(UnorderedBimap &parent, const TValue &value) noexcept
			    : _parent(parent),
			      _value(value)
			{
			}

			operator TValue &()
			{
				return _parent._value2key[_value];
			}

			operator const TValue &() const
			{
				return _parent._value2key[_value];
			}

			ValueProxy &operator=(const TKey &key)
			{
				if (_parent.EraseByValue(_value))
				{
					_parent.Insert(key, _value);
				}
				return *this;
			}
		};

	  private:
		TKeyUnorderedMap _key2value;
		TValueUnorderedMap _value2key;

	  public:
		ValueProxy operator[](const TKey key)
		{
			return ValueProxy(*this, key);
		}

		const ValueProxy operator[](const TKey key) const
		{
			return ValueProxy(*this, key);
		}

		KeyProxy operator[](const TValue value)
		{
			return KeyProxy(*this, value);
		}

		const KeyProxy operator[](const TValue value) const
		{
			return KeyProxy(*this, value);
		}

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

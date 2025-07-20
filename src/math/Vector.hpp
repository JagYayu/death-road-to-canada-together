#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <stdexcept>

#define TUDOV_IMPL_MSG_OUT_OF_RANGE "Vector index out of range"

namespace tudov::impl
{
	template <size_t N, typename T>
	struct VectorData
	{
		std::array<T, N> array;
	};

	template <size_t N, typename T>
	struct VectorImpl : public VectorData<N, T>
	{
		static_assert(N > 0, "Vector dimension must be greater than 0");
		static_assert(std::is_arithmetic_v<T>, "T must be a numeric type");

		static constexpr size_t Dimension = N;

		template <typename... Args>
		inline constexpr explicit VectorImpl(Args &&...args) noexcept
		    : VectorData<N, T>(std::forward<Args>(args)...)
		{
		}

		inline constexpr T &operator[](size_t index)
		{
			if (index < N) [[likely]]
			{
				return this->array[index];
			}
			throw std::out_of_range(TUDOV_IMPL_MSG_OUT_OF_RANGE);
		}

		inline constexpr const T &operator[](size_t index) const
		{
			if (index < N) [[likely]]
			{
				return this->array[index];
			}
			throw std::out_of_range(TUDOV_IMPL_MSG_OUT_OF_RANGE);
		}

		inline constexpr bool operator==(const VectorImpl &other) const noexcept
		{
			for (size_t i = 0; i < N; ++i)
			{
				if (this->array[i] != other.array[i])
				{
					return false;
				}
			}
			return true;
		}

		inline constexpr bool operator!=(const VectorImpl &vec) const noexcept
		{
			return !(*this == vec);
		}

		inline constexpr VectorImpl &operator+=(const VectorImpl &vec) noexcept
		{
			for (size_t i = 0; i < N; ++i)
			{
				this->array[i] += vec.array[i];
			}
			return *this;
		}

		inline constexpr VectorImpl operator+(const VectorImpl &vec) const noexcept
		{
			VectorImpl clone = *this;
			clone += vec;
			return clone;
		}

		inline constexpr VectorImpl &operator-=(const VectorImpl &vec) noexcept
		{
			for (size_t i = 0; i < N; ++i)
			{
				this->array[i] -= vec.array[i];
			}
			return *this;
		}

		inline constexpr VectorImpl operator-(const VectorImpl &vec) const noexcept
		{
			VectorImpl clone = *this;
			clone -= vec;
			return clone;
		}

		inline constexpr VectorImpl &operator*=(const VectorImpl &vec) noexcept
		{
			for (size_t i = 0; i < N; ++i)
			{
				this->array[i] *= vec.array[i];
			}
			return *this;
		}

		inline constexpr VectorImpl &operator*=(T value) noexcept
		{
			for (size_t i = 0; i < N; ++i)
			{
				this->array[i] *= value;
			}
			return *this;
		}

		inline constexpr VectorImpl operator*(const VectorImpl &vec) const noexcept
		{
			VectorImpl clone = *this;
			clone *= vec;
			return clone;
		}

		inline constexpr VectorImpl operator*(T value) const noexcept
		{
			VectorImpl clone = *this;
			clone *= value;
			return clone;
		}

		inline constexpr VectorImpl &operator/=(const VectorImpl &vec) noexcept
		{
			for (size_t i = 0; i < N; ++i)
			{
				this->array[i] /= vec.array[i];
			}
			return *this;
		}

		inline constexpr VectorImpl &operator/=(T value) noexcept
		{
			for (size_t i = 0; i < N; ++i)
			{
				this->array[i] /= value;
			}
			return *this;
		}

		inline constexpr VectorImpl operator/(const VectorImpl &other) const noexcept
		{
			VectorImpl clone = *this;
			clone /= other;
			return clone;
		}

		inline constexpr VectorImpl operator/(T value) const noexcept
		{
			VectorImpl clone = *this;
			clone /= value;
			return clone;
		}

		template <typename U>
		inline constexpr explicit operator VectorImpl<N, U>() const
		{
			VectorImpl<N, U> result{};
			for (size_t i = 0; i < N; ++i)
			{
				result[i] = static_cast<U>(this->array[i]);
			}
			return result;
		}

		template <size_t Index>
		inline constexpr T Get() const noexcept
		{
			static_assert(Index < N, TUDOV_IMPL_MSG_OUT_OF_RANGE);
			return this->array[Index];
		}

		template <size_t Index>
		inline constexpr T Set(const T &value) noexcept
		{
			static_assert(Index < N, TUDOV_IMPL_MSG_OUT_OF_RANGE);
			this->array[Index] = value;
		}

		template <size_t Index>
		inline constexpr T &GetRef() noexcept
		{
			static_assert(Index < N, TUDOV_IMPL_MSG_OUT_OF_RANGE);
			return this->array[Index];
		}

		template <size_t Index>
		inline constexpr const T &GetRef() const noexcept
		{
			static_assert(Index < N, TUDOV_IMPL_MSG_OUT_OF_RANGE);
			return this->array[Index];
		}

		inline constexpr T Dot(const VectorImpl &other) const noexcept
		{
			return std::inner_product(this->array.begin(), this->array.end(), other.begin(), T(0));
		}

		inline void Fill(const T &value) noexcept
		{
			this->array.fill(value);
		}

		inline constexpr T Magnitude() noexcept
		{
			return std::sqrt(SquareMagnitude());
		}

		inline constexpr VectorImpl &Normalize() noexcept
		{
			T length = Magnitude();
			if (length != 0) [[likely]]
			{
				(*this) /= length;
			}
			return *this;
		}

		inline constexpr VectorImpl Normalized() const noexcept
		{
			VectorImpl clone{*this};
			clone.Normalize();
			return clone;
		}

		inline constexpr T SquareMagnitude() noexcept
		{
			T total = 0;
			for (T value : this->array)
			{
				total += value * value;
			}
			return total;
		}

		inline decltype(auto) begin() noexcept
		{
			return this->array.begin();
		}
		inline decltype(auto) end() noexcept
		{
			return this->array.end();
		}
		inline decltype(auto) begin() const noexcept
		{
			return this->array.begin();
		}
		inline decltype(auto) end() const noexcept
		{
			return this->array.end();
		}
		inline decltype(auto) cbegin() const noexcept
		{
			return this->array.cbegin();
		}
		inline decltype(auto) cend() const noexcept
		{
			return this->array.cend();
		}
		inline decltype(auto) rbegin() const noexcept
		{
			return this->array.rbegin();
		}
		inline decltype(auto) rend() const noexcept
		{
			return this->array.rend();
		}
		inline constexpr size_t size() const noexcept
		{
			return N;
		}
	};

	template <typename T>
	struct VectorData<2, T>
	{
		union
		{
			struct
			{
				T x, y;
			};
			std::array<T, 2> array;
		};

		template <typename... Args>
		inline constexpr explicit VectorData(Args &&...args) noexcept
		    : array(std::forward<Args>(args)...){};

		inline constexpr explicit VectorData(T x, T y) noexcept
		    : array({x, y}) {};
	};

	template <typename T>
	struct VectorData<3, T>
	{
		union
		{
			struct
			{
				T x, y, z;
			};
			std::array<T, 3> array;
		};

		template <typename... Args>
		inline constexpr explicit VectorData(Args &&...args) noexcept
		    : array(std::forward<Args>(args)...){};

		inline constexpr explicit VectorData(T x, T y, T z) noexcept
		    : array({x, y, z}) {};
	};

	template <typename T>
	struct VectorData<4, T>
	{
		union
		{
			struct
			{
				T x, y, z, w;
			};
			std::array<T, 4> array;
		};

		template <typename... Args>
		inline constexpr explicit VectorData(Args &&...args) noexcept
		    : array(std::forward<Args>(args)...){};

		inline constexpr explicit VectorData(T x, T y, T z, T w) noexcept
		    : array({x, y, z, w}) {};
	};
} // namespace tudov::impl

namespace tudov
{
	template <size_t Dimension, typename TValue>
	struct Vector : public impl::VectorImpl<Dimension, TValue>
	{
		constexpr explicit Vector() noexcept = default;

		template <typename... Args>
		inline constexpr explicit Vector(Args &&...args) noexcept
		    : impl::VectorImpl<Dimension, TValue>(std::forward<Args>(args)...)
		{
			static_assert(sizeof...(Args) == Dimension, "Number of arguments must match vector dimension");
			static_assert((std::is_same_v<std::decay_t<Args>, TValue> && ...), "All arguments must be of type TValue");
		}
	};

	template <typename TValue>
	struct Vector<2, TValue> : public impl::VectorImpl<2, TValue>
	{
		constexpr explicit Vector() noexcept = default;

		template <typename... Args>
		inline constexpr explicit Vector(Args &&...args) noexcept
		    : impl::VectorImpl<2, TValue>(std::forward<Args>(args)...)
		{
			static_assert(sizeof...(Args) == 2, "Number of arguments must match vector dimension");
			static_assert((std::is_same_v<std::decay_t<Args>, TValue> && ...), "All arguments must be of type TValue");
		}

		inline constexpr explicit Vector(TValue x = 0, TValue y = 0) noexcept
		    : impl::VectorImpl<2, TValue>(x, y)
		{
		}
	};

	template <typename TValue>
	struct Vector<3, TValue> : public impl::VectorImpl<3, TValue>
	{
		// TODO
	};

	template <typename TValue>
	struct Vector<4, TValue> : public impl::VectorImpl<4, TValue>
	{
		// TODO
	};

	template <typename TValue = std::int32_t>
	using Vector2 = Vector<2, TValue>;
	using Vector2F = Vector<2, std::float_t>;
	using Vector2L = Vector<2, std::int64_t>;
	using Vector2D = Vector<2, std::double_t>;

	template <typename TValue = std::int32_t>
	using Vector3 = Vector<3, TValue>;
	using Vector3F = Vector<3, std::float_t>;
	using Vector3L = Vector<3, std::int64_t>;
	using Vector3D = Vector<3, std::double_t>;

	template <typename TValue = std::int32_t>
	using Vector4 = Vector<4, TValue>;
	using Vector4F = Vector<4, std::float_t>;
	using Vector4L = Vector<4, std::int64_t>;
	using Vector4D = Vector<4, std::double_t>;
} // namespace tudov

#undef TUDOV_IMPL_MSG_OUT_OF_RANGE

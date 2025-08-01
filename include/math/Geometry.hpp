#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <vector>

#define TUDOV_IMPL_MSG_OUT_OF_RANGE "Geometry index out of range"

namespace tudov::impl
{
	template <size_t N, typename T>
	struct GeometryData
	{
		union
		{
			struct
			{
				std::array<T, N> point;
				std::array<T, N> sizes;
			};
			std::array<T, N * 2> array;
		};
	};

	template <size_t N, typename T>
	struct GeoImpl : public GeometryData<N, T>
	{
		static_assert(N > 0, "Geometry dimension must be greater than 0");
		static_assert(std::is_arithmetic_v<T>, "Value must be a numeric type");

		static constexpr size_t Dimension = N;

		template <typename... Args>
		inline constexpr explicit GeoImpl(Args &&...args) noexcept
		    : GeometryData<N, T>(std::forward<Args>(args)...)
		{
		}

		inline constexpr bool operator==(const GeoImpl &other) const noexcept
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

		inline constexpr bool operator!=(const GeoImpl &vec) const noexcept
		{
			return !(*this == vec);
		}

		template <typename U>
		inline constexpr explicit operator GeoImpl<N, U>() const
		{
			GeoImpl<N, U> result{};
			for (size_t i = 0; i < N; ++i)
			{
				result[i] = static_cast<U>(this->array[i]);
			}
			return result;
		}

		inline constexpr std::vector<T> Origin() const noexcept
		{
			return std::vector<T>(this->point);
		}

		inline constexpr T Size() const noexcept
		{
			T size = 1;
			for (auto value : this->sizes)
			{
				size *= value;
			}
			return size;
		}
	};

	template <typename T>
	struct GeometryData<1, T>
	{
		union
		{
			struct
			{
				T x;
				T w;
			};
			std::array<T, 4> array;
		};

		template <typename... Args>
		inline constexpr explicit GeometryData(Args &&...args) noexcept
		    : array(std::forward<Args>(args)...){};

		inline constexpr explicit GeometryData(T x, T y) noexcept
		    : array({x, y}) {};
	};

	template <typename T>
	struct GeometryData<2, T>
	{
		union
		{
			struct
			{
				T x, y;
				T w, h;
			};
			std::array<T, 4> array;
		};

		template <typename... Args>
		inline constexpr explicit GeometryData(Args &&...args) noexcept
		    : array(std::forward<Args>(args)...){};

		inline constexpr explicit GeometryData(T x, T y) noexcept
		    : array({x, y}) {};
	};

	template <typename T>
	struct GeometryData<3, T>
	{
		union
		{
			struct
			{
				T x, y, z;
				T w, h, l;
			};
			std::array<T, 6> array;
		};

		template <typename... Args>
		inline constexpr explicit GeometryData(Args &&...args) noexcept
		    : array(std::forward<Args>(args)...){};

		inline constexpr explicit GeometryData(T x, T y, T z, T w, T h, T l) noexcept
		    : array({x, y, z, w, h, l}) {};
	};
} // namespace tudov::impl

namespace tudov
{
	template <size_t Dimension, typename TValue>
	struct Geo : public impl::GeoImpl<Dimension, TValue>
	{
		constexpr explicit Geo() noexcept = default;

		template <typename... Args>
		inline constexpr explicit Geo(Args &&...args) noexcept
		    : impl::GeoImpl<Dimension, TValue>(std::forward<Args>(args)...)
		{
			static_assert(sizeof...(Args) == Dimension, "Number of arguments must match geometry dimension");
			static_assert((std::is_same_v<std::decay_t<Args>, TValue> && ...), "All arguments must be of type TValue");
		}
	};

	template <typename TValue>
	struct Geo<1, TValue> : public impl::GeoImpl<1, TValue>
	{
		constexpr explicit Geo() noexcept = default;

		template <typename... Args>
		inline constexpr explicit Geo(Args &&...args) noexcept
		    : impl::GeoImpl<2, TValue>(std::forward<Args>(args)...)
		{
			static_assert(sizeof...(Args) == 2, "Number of arguments must match vector dimension");
			static_assert((std::is_same_v<std::decay_t<Args>, TValue> && ...), "All arguments must be of type TValue");
		}

		inline constexpr explicit Geo(TValue x = 0, TValue w = 0) noexcept
		    : impl::GeoImpl<2, TValue>(x, w)
		{
		}
	};

	template <typename TValue>
	struct Geo<2, TValue> : public impl::GeoImpl<2, TValue>
	{
		constexpr explicit Geo() noexcept = default;

		template <typename... Args>
		inline constexpr explicit Geo(Args &&...args) noexcept
		    : impl::GeoImpl<2, TValue>(std::forward<Args>(args)...)
		{
			static_assert(sizeof...(Args) == 4, "Number of arguments must match vector dimension");
			static_assert((std::is_same_v<std::decay_t<Args>, TValue> && ...), "All arguments must be of type TValue");
		}

		inline constexpr explicit Geo(TValue x = 0, TValue y = 0, TValue w = 0, TValue h = 0) noexcept
		    : impl::GeoImpl<2, TValue>(x, y, w, h)
		{
		}
	};

	template <typename TValue>
	struct Geo<3, TValue> : public impl::GeoImpl<3, TValue>
	{
		// TODO
	};

	template <typename TValue = std::int32_t>
	using Segment2 = Geo<2, TValue>;
	using Segment2F = Geo<2, std::float_t>;
	using Segment2L = Geo<2, std::int64_t>;
	using Segment2D = Geo<2, std::double_t>;

	template <typename TValue = std::int32_t>
	using Rectangle2 = Geo<2, TValue>;
	using Rectangle2F = Geo<2, std::float_t>;
	using Rectangle2L = Geo<2, std::int64_t>;
	using Rectangle2D = Geo<2, std::double_t>;
} // namespace tudov

#undef TUDOV_IMPL_MSG_OUT_OF_RANGE

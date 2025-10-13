/**
 * @file math/Matrix.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <cmath>
#include <initializer_list>
#include <stdexcept>

namespace tudov
{
	template <size_t VRows, size_t VColumns, typename TValue = std::float_t>
	struct Matrix
	{
		static_assert(VRows > 0 && VColumns > 0, "Matrix rows & columns must be greater than 0");
		static_assert(std::is_floating_point_v<TValue>, "Value type must be a floating point type");

		static constexpr size_t Rows = VRows;
		static constexpr size_t Columns = VColumns;

		std::array<TValue, Rows * Columns> data_;

		inline constexpr explicit Matrix() noexcept
		    : data_{}
		{
		}

		inline constexpr explicit Matrix(std::initializer_list<std::initializer_list<TValue>> init)
		{
			if (init.size() != Rows) [[unlikely]]
			{
				throw std::invalid_argument("Invalid number of rows in initializer list");
			}

			size_t i = 0;
			for (const auto &row : init)
			{
				if (row.size() != Columns)
				{
					throw std::invalid_argument("Invalid number of columns in initializer list");
				}
				size_t j = 0;
				for (const auto &val : row)
				{
					data_[i * Columns + j] = val;
					++j;
				}
				++i;
			}
		}

		inline constexpr TValue &operator()(size_t row, size_t col)
		{
			if (row < Rows && col < Columns) [[likely]]
			{
				return data_[row * Columns + col];
			}
			throw std::out_of_range("Matrix indices out of range");
		}

		inline constexpr const TValue &operator()(size_t row, size_t col) const
		{
			if (row < Rows && col < Columns) [[likely]]
			{
				return data_[row * Columns + col];
			}
			throw std::out_of_range("Matrix indices out of range");
		}

		inline constexpr Matrix operator+(const Matrix &other) const
		{
			Matrix result;
			for (size_t i = 0; i < Rows * Columns; ++i)
			{
				result.data_[i] = data_[i] + other.data_[i];
			}
			return result;
		}

		inline constexpr Matrix operator-(const Matrix &other) const
		{
			Matrix result;
			for (size_t i = 0; i < Rows * Columns; ++i)
			{
				result.data_[i] = data_[i] - other.data_[i];
			}
			return result;
		}

		inline constexpr Matrix operator*(TValue scalar) const
		{
			Matrix result;
			for (size_t i = 0; i < Rows * Columns; ++i)
			{
				result.data_[i] = data_[i] * scalar;
			}
			return result;
		}

		template <size_t OtherColumns>
		inline constexpr Matrix<Rows, OtherColumns, TValue> operator*(const Matrix<Columns, OtherColumns, TValue> &other) const
		{
			Matrix<Rows, OtherColumns, TValue> result;
			for (size_t i = 0; i < Rows; ++i)
			{
				for (size_t j = 0; j < OtherColumns; ++j)
				{
					TValue sum = 0;
					for (size_t k = 0; k < Columns; ++k)
					{
						sum += (*this)(i, k) * other(k, j);
					}
					result(i, j) = sum;
				}
			}
			return result;
		}

		inline constexpr Matrix<Columns, Rows, TValue> transpose() const
		{
			Matrix<Columns, Rows, TValue> result;
			for (size_t i = 0; i < Rows; ++i)
			{
				for (size_t j = 0; j < Columns; ++j)
				{
					result(j, i) = (*this)(i, j);
				}
			}
			return result;
		}

		inline constexpr size_t GetRows() const
		{
			return Rows;
		}

		inline constexpr size_t GetColumns() const
		{
			return Columns;
		}
	};

	template <size_t Rows, size_t Columns, typename TValue>
	inline constexpr Matrix<Rows, Columns, TValue> operator*(TValue scalar, const Matrix<Rows, Columns, TValue> &matrix)
	{
		return matrix * scalar;
	}

	template <size_t N, typename TValue>
	inline constexpr Matrix<N, N, TValue> identity()
	{
		Matrix<N, N, TValue> matrix;
		for (size_t i = 0; i < N; ++i)
		{
			matrix(i, i) = 1;
		}
		return matrix;
	};

	template <typename TValue = std::float_t>
	using Matrix3x3 = Matrix<3, 3, TValue>;
	template <typename TValue = std::float_t>
	using Matrix4x4 = Matrix<4, 4, TValue>;
} // namespace tudov

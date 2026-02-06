#pragma once
#include <cassert>
#include <array>
#include <type_traits>
#include <utility>
#include <cmath>

// This is some bullshit
// #define EPSILON numeric_limits<T>::epsilon()
#define EPSILON 1e-06F

namespace Math3D {
	using namespace std;

	template <typename T, size_t W, size_t H>
	struct Matrix;

	// TODO should not live in Matrix.cpp long term
	template <typename T>
	constexpr T lerp(const T& a, const T& b, float t) {
		return (1 - t) * a + t * b;
	}

	template <typename T>
	bool nearly_equal(const T& lhs, const T& rhs) {
		return abs(lhs - rhs) < EPSILON;
	}

	template<class T, class ... ArgTypes>
	concept Assignable = requires() {
		conjunction_v<is_assignable<T, ArgTypes>...> && sizeof...(ArgTypes) > 1;
	};

	template <typename T, size_t W, size_t H>
	struct Matrix {
		using this_t = Matrix<T, W, H>;
		using row_t  = Matrix<T, W, 1>;
		using col_t  = Matrix<T, 1, H>;

		static constexpr const size_t N = W * H;
		static constexpr const auto Seq_Data = make_index_sequence<N>();
		static constexpr const auto Seq_Row  = make_index_sequence<W>();
		static constexpr const auto Seq_Col  = make_index_sequence<H>();

		static constexpr this_t identity() {
			return identity_impl(Seq_Data);
		}

		template<size_t ... Seq> 
		static constexpr this_t identity_impl(const index_sequence<Seq...>&) {
			return this_t(std::array<T, N>{ ((Seq / W == Seq % W) ? (1) : (0))... });
		}

		Matrix() requires(is_same_v<this_t, Matrix<float, 3, 4>>);

		Matrix() : arr{0} {}

		Matrix(const T& val) {arr.fill(val); }

		constexpr Matrix(const row_t (&vecs)[H]) { vector_constructor_impl(vecs, Seq_Col); }

		template <typename ... ArgTypes>
		constexpr Matrix(ArgTypes ... args) requires (Assignable<T, ArgTypes...>): arr{args...} { static_assert(sizeof...(args) <= N); }
		constexpr Matrix(const array<T, N>& _arr) : arr(_arr) {}
		constexpr this_t& operator= (const this_t& val) { arr = val.arr; return *this; }
		constexpr bool operator==(const this_t& val) const { return arr == val.arr; }
		constexpr conditional_t<H == 1, T, row_t>& operator[](size_t i) { return vec[i]; }
		constexpr conditional_t<H == 1, T, row_t> operator[](size_t i) const { return vec[i]; }
		constexpr this_t operator+(const T& val) const { return scalar_add_impl(val, Seq_Data); }
		constexpr this_t operator*(const T& val) const { return scalar_mul_impl(val, Seq_Data); }
		constexpr this_t operator/(const T& val) const { return scalar_div_impl(val, Seq_Data); }

		constexpr bool nearly_equal(const this_t& rhs) const {
			return nearly_equal_impl(rhs, Seq_Data);
		}

		this_t& operator+=(const T& val) {
			this_t& _this = *this;
			_this = _this + val;
			return _this;
		}

		this_t& operator++() {
			return *this += 1;
		}

		this_t operator++(int) {
			this_t& _this = *this;
			this_t val = _this;
			_this += 1;
			return val;
		}

		constexpr this_t operator+(const this_t& val) const {
			return vector_add_impl(val, Seq_Data);
		}

		constexpr this_t operator-(const this_t& val) const {
			return vector_sub_impl(val, Seq_Data);
		}

		// Special case for Xformf * Xformf: treat as 4x4 with implied 4th column (0,0,0,1)
		constexpr Matrix<T, 3, 4> operator*(const Matrix<T, 3, 4>& val) const requires (W == 3 && H == 4) {
			this_t result;

			// Treat the first 3 rows as a 3x3 submatrix
			(Matrix<T, 3, 3>&)(result) = (Matrix<T, 3, 3>&)(*this) * (Matrix<T, 3, 3>&)(val);
			Matrix<T, 4, 1> position {data[3][0], data[3][1], data[3][2], 1.0f};

			// Compute the bottom row: 3x4 transform * (val with implicit [0,0,0,1] bottom row)
			result[3] = row_t { position.dot(val.col(0)), position.dot(val.col(1)), position.dot(val.col(2)) };
			return result;
		}

		constexpr Matrix<T, 4, 4> operator*(const Matrix<T, 4, 4>& val) const requires (W == 3 && H == 4) {
			Matrix<T, 4, 4> result;
			Matrix<T, 4, 1> position {data[3][0], data[3][1], data[3][2], 1.0f};

			// Treat the first 3 rows as a 3x4 submatrix
			(Matrix<T, 3, 3>&)(result) = (Matrix<T, 3, 3>&)(*this) * (Matrix<T, 3, 3>&)(val);
			
			// Compute the bottom row: 3x4 transform * (val with implicit [0,0,0,1] bottom row)
			result[3] = Matrix<T, 4, 1> { position.dot(val.col(0)), position.dot(val.col(1)), position.dot(val.col(2)), position.dot(val.col(3)) };
			return result;
		}

		template <class _T, size_t _W, size_t _H>
		constexpr Matrix<T, _W, H> operator*(const Matrix<_T, _W, _H>& val) const /*requires (W == _W && H == _H)*/ {
			static_assert(W == _H);
			return matrix_mul_impl(val, Seq_Row);
		}

		this_t operator+=(const this_t& val) {
			this_t& _this = *this;
			_this = _this + val;
			return _this;
		}

		template <class _T, size_t _W, size_t _H>
		constexpr T dot(const Matrix<_T, _W, _H>& val) const {
			static_assert(N == Matrix<_T, _W, _H>::N);
			return inner_product_impl(val, Seq_Data);
		}

		constexpr row_t row(size_t i) const {
			return row_t(data[i]);
		}

		constexpr col_t col(size_t i) const {
			return col_impl(i, Seq_Col);
		}

		constexpr this_t lerp(const this_t& val, float t) const {
			return lerp_impl(val, t, Seq_Data);
		}

		constexpr T length() const {
			return magnitude_impl(Seq_Data);
		}

		constexpr this_t normalize() const {
			return *this / length();
		}

		constexpr this_t cross(const this_t& val) const requires (is_same_v<this_t, Matrix<T, 3, 1>>) {
			return row_t(
				val[1] * vec[2] - val[2] * vec[1],
				val[2] * vec[0] - val[0] * vec[2],
				val[0] * vec[1] - val[1] * vec[0]
			);
		}

		constexpr T determinant() const requires (W == H) {
			if constexpr (W == 1) {
				return data[0][0];
			} 
			else if constexpr (W == 2) {
				return data[0][0] * data[1][1] - data[0][1] * data[1][0];
			} 
			else {
				return determinant_impl(Seq_Row, make_adjoint_sign_sequence(Seq_Row));
			}
		}

		constexpr Matrix<T, H, W> transpose() {
			return transpose_impl(Seq_Row);
		}

		constexpr T trace() const requires (W == H) {
			return trace_impl(Seq_Row);
		}

		constexpr Matrix<T, W, H - 1> remove_row(size_t i) const {
			return remove_row_impl(i, Seq_Row);
		}

		constexpr Matrix<T, W - 1, H> remove_column(size_t i) const {
			return remove_column_impl(i, Seq_Col);
		}

		constexpr this_t adjoint() const {
			return adjoint_impl(Seq_Data, make_adjoint_sign_sequence(Seq_Data));
		}

		constexpr this_t inverse() const {
			T det = determinant();
			assert(det != 0);

			return adjoint() * (1 / det);
		}

		union {
			array<T, N> arr;
			array<array<T, W>, H> data;
			conditional_t<H == 1, array<T, W>, array<row_t, H>> vec;
		};

	private:
		template <size_t ... Seq>
		constexpr this_t scalar_add_impl(const T& val, const index_sequence<Seq...>& = Seq_Data) const {
			return {(arr[Seq] + val) ...};
		}

		template <size_t ... Seq>
		constexpr this_t scalar_mul_impl(const T& val, const index_sequence<Seq...>& = Seq_Data) const {
			return {(arr[Seq] * val) ...};
		}

		template <size_t ... Seq>
		constexpr this_t vector_add_impl(const this_t& val, const index_sequence<Seq...>&) const {
			return {(arr[Seq] + val.arr[Seq]) ...};
		}

		template <size_t ... Seq>
		constexpr this_t vector_sub_impl(const this_t& val, const index_sequence<Seq...>&) const {
			return {(arr[Seq] - val.arr[Seq]) ...};
		}


		template <size_t ... Seq>
		constexpr this_t scalar_div_impl(const T& val, const index_sequence<Seq...>& = Seq_Data) const {
			return {(arr[Seq] / val) ...};
		}

		template <class _T, size_t _W, size_t _H, size_t ... Seq>
		constexpr T inner_product_impl(const Matrix<_T, _W, _H>& val, const index_sequence<Seq...>&) const {
			return (0 + ... + (arr[Seq] * val.arr[Seq]));
		}

		template <class _T, size_t _W, size_t _H, size_t ... Seq>
		constexpr Matrix<T, _W, H> matrix_mul_impl(const Matrix<_T, _W, _H>& val, const index_sequence<Seq...>&) const {
			Matrix<T, _W, H> out;
			((out[Seq] = matrix_mul_impl_inner(val, Seq, Seq_Col)), ...);
			return out;
		}

		template <class _T, size_t _W, size_t _H, size_t ... Seq>
		constexpr Matrix<T, _W, 1> matrix_mul_impl_inner(const Matrix<_T, _W, _H>& val, size_t i, const index_sequence<Seq...>&) const {
			return Matrix<T, _W, 1>(row(i).dot(val.col(Seq)) ...);
		}

		template <size_t ... Seq>
		constexpr void vector_constructor_impl(const row_t (&vecs)[H], const index_sequence<Seq...>&) {
			((vector_constructor_inner_impl(vecs[Seq], Seq, Seq_Row)), ...);
		}

		template <size_t ... Seq>
		constexpr void vector_constructor_inner_impl(const row_t &vec, const size_t i, const index_sequence<Seq...>&) {
			((data[i][Seq] = vec[Seq]), ...);
		}

		template <size_t ... Seq>
		constexpr col_t col_impl(size_t i, const index_sequence<Seq...>&) const {
			return col_t(data[Seq][i] ...);
		}

		template <size_t ... Seq>
		constexpr this_t lerp_impl(const this_t& val, float t, const index_sequence<Seq...>&) const {
			return this_t((Math3D::lerp(arr[Seq], val.arr[Seq], t)) ...);
		}

		template <size_t ... Seq>
		constexpr T magnitude_impl(const index_sequence<Seq...>&) const {
			return std::sqrt((0 + ... + static_cast<T>(pow(arr[Seq], 2))));
		}

		template <size_t ... Seq>
		constexpr T trace_impl(const index_sequence<Seq...>& seq) const {
			return (0 + ... + data[Seq][Seq]);
		}

		template<size_t ... ColSeq>
		constexpr Matrix<T, W, H - 1> remove_row_impl(size_t row, const index_sequence<ColSeq...>&) const {
			Matrix<T, W, H - 1> result;
			((remove_row_impl_inner(result, row, ColSeq, make_index_sequence<H - 1>())), ...);
			return result;
		}

		template<size_t ... NewRowSeq>
		constexpr void remove_row_impl_inner(Matrix<T, W, H - 1>& result, size_t row_to_remove, size_t col, const index_sequence<NewRowSeq...>&) const {
			((result.data[NewRowSeq][col] = data[NewRowSeq < row_to_remove ? NewRowSeq : NewRowSeq + 1][col]), ...);
		}

		template<size_t ... RowSeq>
		constexpr Matrix<T, W - 1, H> remove_column_impl(size_t col, const index_sequence<RowSeq...>&) const {
			Matrix<T, W - 1, H> result;
			((remove_column_impl_inner(result, RowSeq, col, make_index_sequence<W - 1>())), ...);
			return result;
		}

		template<size_t ... ColSeq>
		constexpr void remove_column_impl_inner(Matrix<T, W - 1, H>& result, size_t row, size_t col, const index_sequence<ColSeq...>&) const {
			((result.data[row][ColSeq] = data[row][ColSeq < col ? ColSeq : ColSeq + 1]), ...);
		}

		template<size_t ... DataSeq, int ... SignSeq>
		constexpr T determinant_impl(const index_sequence<DataSeq...>&, const integer_sequence<int, SignSeq...>&) const {
			auto minor_base = remove_row(0);
			
			return ((
				minor_base.remove_column(DataSeq).determinant() // Minor
				* data[0][DataSeq] // Cofactor
				* SignSeq // Sign
			) + ... + 0);
		}

		template<size_t ... Seq>
		constexpr Matrix<T, H, W> transpose_impl(const index_sequence<Seq...>&) const {
			Matrix<T, H, W> result;
			((result.vec[Seq] = transpose_impl_inner(Seq, Seq_Col)), ...);
			return result;
		}

		template<size_t ... Seq>
		constexpr Matrix<T, H, 1> transpose_impl_inner(size_t i, const index_sequence<Seq...>&) const {
			return Matrix<T, H, 1>(data[Seq][i] ...);
		}

		template <size_t... Is>
		static constexpr auto make_adjoint_sign_sequence(index_sequence<Is...>) {
			return integer_sequence<int, ((Is % W + Is / W) % 2 ? -1 : 1)...>{};
		}

		template <size_t ... DataSeq, int ... SignSeq>
		constexpr this_t adjoint_impl(const index_sequence<DataSeq...>&, const integer_sequence<int, SignSeq...>&) const {
			// Compute flattened adjoint by mapping linear index to (row, col) pairs
			// Adjoint is transpose of cofactor, so for linear index i:
			// adjoint[i/W][i%W] = cofactor[i%W][i/W] = (-1)^(i%W + i/W) * det(minor(i%W, i/W))
			return this_t((
				remove_row(DataSeq % W).remove_column(DataSeq / W).determinant() * SignSeq
			) ...);
		}

		template<size_t ... Seq>
		constexpr bool nearly_equal_impl(const this_t& rhs, const index_sequence<Seq...>&) const {
			bool result = (Math3D::nearly_equal(arr[Seq], rhs.arr[Seq]) && ...);
			return result;
		}
	};

	template<typename T, size_t W, size_t H>
	bool nearly_equal(const Matrix<T, W, H>& lhs, const Matrix<T, W, H>& rhs) {
		return lhs.nearly_equal(rhs);
	}

	template<typename T, size_t W> using Vec = Matrix<T, W, 1>;
	using Vec2f = Vec<float, 2>;
	using Vec3f = Vec<float, 3>;
	using Vec4f = Vec<float, 4>;
	using Mat2f = Matrix<float, 2, 2>;
	using Mat3f = Matrix<float, 3, 3>;
	using Mat4f = Matrix<float, 4, 4>;
	using Xformf = Matrix<float, 3, 4>;

	constexpr const Xformf Identity = Xformf::identity();

	template <typename T, size_t W, size_t H>
	Matrix<T, W, H>::Matrix() requires(is_same_v<this_t, Matrix<float, 3, 4>>) {*this = Identity;}
}

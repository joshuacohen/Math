#pragma once
#include <array>
#include <type_traits>
#include <utility>

namespace Math3D {
	using namespace std;

	template <typename T, size_t W, size_t H>
	struct Matrix;

	// TODO should not live in Matrix.cpp long term
	template <typename T>
	constexpr T lerp(const T& a, const T& b, float t) {
		return (1 - t) * a + t * b;
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

		Matrix() requires(is_same_v<this_t, Matrix<float, 3, 4>>);

		Matrix() : arr{0} {}

		Matrix(const T& val) {arr.fill(val); }

		template <typename ... ArgTypes>
		constexpr Matrix(ArgTypes ... args) requires (Assignable<T, ArgTypes...>): arr{args...} { static_assert(sizeof...(args) <= N); }
		constexpr Matrix(const array<T, N>& _arr) : arr(_arr) {}
		constexpr Matrix(const row_t (&vecs)[H]) { vector_constructor_impl(vecs, Seq_Col); }
		constexpr this_t& operator= (const this_t& val) { arr = val.arr; return *this; }
		constexpr bool operator==(const this_t& val) const { return arr == val.arr; }
		constexpr conditional_t<H == 1, T, row_t>& operator[](size_t i) { return vec[i]; }
		constexpr conditional_t<H == 1, T, row_t> operator[](size_t i) const { return vec[i]; }
		constexpr this_t operator+(const T& val) const { return scalar_add_impl(val, Seq_Data); }
		constexpr this_t operator*(const T& val) const { return scalar_mul_impl(val, Seq_Data); }
		constexpr this_t operator/(const T& val) const { return scalar_div_impl(val, Seq_Data); }


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

		template <class _T, size_t _W, size_t _H>
		constexpr Matrix<T, _W, H> operator*(const Matrix<_T, _W, _H>& val) const {
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
				vec[1] * val[2] - vec[2] * val[1],
				vec[2] * val[0] - vec[0] * val[2],
				vec[0] * val[1] - vec[1] * val[0]
			);
		}

		constexpr T trace() const requires (W == H) {
			return trace_impl(Seq_Row);
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
			return sqrt((0 + ... + static_cast<T>(pow(arr[Seq], 2))));
		}

		template <size_t ... Seq>
		constexpr T trace_impl(const index_sequence<Seq...>& seq) const {
			return (0 + ... + data[Seq][Seq]);
		}
	};

	template<typename T, size_t W> using Vec = Matrix<T, W, 1>;
	using Vec2f = Vec<float, 2>;
	using Vec3f = Vec<float, 3>;
	using Vec4f = Vec<float, 4>;
	using Mat2f = Matrix<float, 2, 2>;
	using Mat3f = Matrix<float, 3, 3>;
	using Mat4f = Matrix<float, 4, 4>;
	using Xformf = Matrix<float, 3, 4>;

	constexpr const Xformf Identity {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f
	};

	template <typename T, size_t W, size_t H>
	Matrix<T, W, H>::Matrix() requires(is_same_v<this_t, Matrix<float, 3, 4>>) {*this = Identity;}
}

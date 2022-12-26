/*
 * Copyright (c) 2022 Vladimir Kozelkov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef INTEGER_HPP
#define INTEGER_HPP

#include <type_traits>

#if __has_include(<ostream>)
#include <ostream>
#define INTEGER_HPP_HAS_OSTREAM
#endif

#ifdef __has_builtin
#define INTEGER_HPP_HAS_BUILTIN_CHECK
#else
#define __has_builtin(unused) 0
#endif

// -std=c++17
namespace JIO {
    namespace i_detail {

        using size_t = decltype(sizeof (0));
        using signed_size_t = typename std::make_signed<size_t>::type;

        template<typename T>
        using size_of = std::integral_constant<size_t, sizeof (T)>;

        template <typename, //it is necessary to trick the compiler
        typename T>
        constexpr inline T instantiation_context(T value) noexcept {
            return value;
        }

        template <typename, //it is necessary to trick the compiler
        typename T, T value>
        constexpr inline T instantiation_context(std::integral_constant<T, value>) noexcept {
            return value;
        }

        namespace micro_std {
            template<typename T, typename TT = std::remove_reference_t<T>>
            using forward_ret_t = std::conditional_t<std::is_lvalue_reference_v<T>, TT&, TT&&>;

            template<typename T, typename TT>
            inline forward_ret_t<T> forward(TT &&v) noexcept {
                return static_cast<T&&> (v);
            }

            template<typename...>
            struct or_t;

            template<>
            struct or_t<> : std::false_type {
            };

            template<typename B1>
            struct or_t<B1> : B1 {
            };

            template<typename B1, typename B2>
            struct or_t<B1, B2> : std::conditional_t<B1::value, B1, B2> {
            };

            template<typename B1, typename B2, typename B3, typename... Bn>
            struct or_t<B1, B2, B3, Bn...>
            : std::conditional_t<B1::value, B1, or_t<B2, B3, Bn...> > {
            };

            template<typename...>
            struct and_t;

            template<>
            struct and_t<> : std::true_type {
            };

            template<typename B1>
            struct and_t<B1> : B1 {
            };

            template<typename B1, typename B2>
            struct and_t<B1, B2> : std::conditional_t<B1::value, B2, B1> {
            };

            template<typename B1, typename B2, typename B3, typename... Bn>
            struct and_t<B1, B2, B3, Bn...>
            : std::conditional_t<B1::value, and_t<B2, B3, Bn...>, B1> {
            };

            template<typename... B>
            using conjunction = and_t<B...>;

            template<typename... B>
            using disjunction = or_t<B...>;

            template<typename B>
            using negation = std::bool_constant<!bool(B::value)>;

            template<typename... B>
            constexpr inline bool conjunction_v = conjunction<B...>::value;

            template<typename... B>
            constexpr inline bool disjunction_v = disjunction<B...>::value;

            template<typename B>
            constexpr inline bool negation_v = negation<B>::value;

            template<typename T>
            struct remove_const {
                using type = T;
            };

            template<typename T>
            struct remove_const<const T> {
                using type = T;
            };

            template<typename T>
            using remove_const_t = typename remove_const<T>::type;

            template<typename T>
            struct remove_volatile {
                using type = T;
            };

            template<typename T>
            struct remove_volatile<volatile T> {
                using type = T;
            };

            template<typename T>
            using remove_volatile_t = typename remove_volatile<T>::type;

            template<typename T>
            struct add_const {
                using type = const T;
            };

            template<typename T>
            using add_const_t = typename add_const<T>::type;

            template<typename T>
            struct add_volatile {
                using type = volatile T;
            };

            template<typename T>
            using add_volatile_t = typename add_volatile<T>::type;
        }

        namespace seq {

            struct ignore_t {

                template <typename... T>
                constexpr ignore_t(T&&...) noexcept { }
            };

            template<auto>
            using ignore_template_t = ignore_t;

            template <typename...>
            struct t_array;

            template <typename T, T...>
            struct c_array;

            template<typename T, T... v1, T... v2>
            c_array<T, v1..., v2...>
            append(c_array<T, v1...>, c_array<T, v2...>);

            template<typename... Tp1, typename... Tp2>
            t_array<Tp1..., Tp2...>
            append(t_array<Tp1...>, t_array<Tp2...>);

            template<typename Arr1, typename Arr2>
            using append_t = decltype(append(Arr1(), Arr2()));

            template <typename T, T... values>
            struct c_array {
                using value_type = T;
                constexpr static size_t length = sizeof...(values);

                constexpr static T get(size_t index) noexcept {
                    constexpr T data[length == 0 ? 1 : length] = {values...};
                    return data[index];
                }

                template<size_t index>
                using get_constant = std::integral_constant<T, get(index)>;

                template<template<typename P, P...> typename X>
                using transform_all = X<T, values...>;

                template<template<auto...> typename X>
                using transform_all_auto = X<values...>;

                template<typename V, template<typename P, P> typename X>
                using transform_c = c_array<V, X<T, values>::value...>;

                template<typename V, template<auto> typename X>
                using transform_c_auto = c_array<V, X<values>::value...>;

                template<template<typename P, P> typename X>
                using transform_t = t_array<X<T, values>...>;

                template<template<auto> typename X>
                using transform_t_auto = t_array<X<values>...>;

                constexpr T operator[](size_t index) const noexcept {
                    return get(index);
                }

                constexpr inline static signed_size_t index_of(T value) noexcept {
                    for (size_t i = 0; i < length; i++) {
                        if (get(i) == value) {
                            return i;
                        }
                    }
                    return -1;
                }

                template<T value>
                using index_of_constant = std::integral_constant<size_t, index_of(value)>;

                constexpr inline static bool contains(T value) noexcept {
                    return index_of(value) != -1;
                }

                template<T value>
                using contains_constant = std::bool_constant<contains(value)>;
            };

            template<typename Arr, template<typename P, P...> typename X>
            using c_transform_all = typename Arr::template transform_all<X>;

            template<typename Arr, template<auto...> typename X>
            using c_transform_all_auto = typename Arr::template transform_all_auto<X>;

            template<typename Arr, typename V, template<typename P, P> typename X>
            using c_transform_c = typename Arr::template transform_c<V, X>;

            template<typename Arr, typename V, template<auto> typename X>
            using c_transform_c_auto = typename Arr::template transform_c_auto<V, X>;

            template<typename Arr, template<typename P, P> typename X>
            using c_transform_t = typename Arr::template transform_t<X>;

            template<typename Arr, template<auto> typename X>
            using c_transform_t_auto = typename Arr::template transform_t_auto<X>;

            template <size_t... values>
            using index_seq = c_array<size_t, values...>;

            template <typename T, size_t size>
            struct v_array {
                using value_type = T;
                constexpr static size_t length = size;
                T data[length == 0 ? 1 : length];

                constexpr const T& operator[](size_t index) const noexcept {
                    return data[index];
                }

                constexpr T& operator[](size_t index) noexcept {
                    return data[index];
                }

                constexpr inline signed_size_t index_of(T value) const noexcept {
                    for (size_t i = 0; i < length; i++) {
                        if (data[i] == value) {
                            return i;
                        }
                    }
                    return -1;
                }

                constexpr inline bool contains(T value) const noexcept {
                    return index_of(value) != -1;
                }
            };

            template<typename T, T low, T high>
            constexpr inline auto make_seq_h() noexcept {
                static_assert(!(high < low), "high < low");
                if constexpr (high == low) {
                    return c_array<T>();
                }
                constexpr T d = high - low;
                if constexpr (d == 1) {
                    return c_array<T, low>();
                }
                return decltype(append(
                        make_seq_h<T, low, low + d / 2 > (),
                        make_seq_h<T, low + d / 2, low + d > ()
                        ))();
            }

            //logarithmic algorithm
            template<typename T, T low, T high>
            using make_seq = decltype(make_seq_h<T, low, high>());

            template<size_t low, size_t high>
            using make_index_seq = make_seq<size_t, low, high>;

            template<size_t... seq>
            struct pack_element_h {

                template <typename T, typename... O>
                constexpr static decltype(auto) helper(
                        ignore_template_t<seq>..., T&& out, O&&...) noexcept {
                    return micro_std::forward<decltype(out)>(out);
                }
            };

            template<size_t index, typename... Tp>
            constexpr inline decltype(auto) pack_element(Tp&&... arr) noexcept {
                using ct = c_transform_all_auto<make_index_seq<0, index>, pack_element_h>;
                return ct::helper(micro_std::forward<Tp>(arr)...);
            }

            template<typename T>
            using type_container = std::enable_if<true, T>;

            template<size_t index, typename... Tp>
            using pack_element_t = typename std::remove_reference_t<
            decltype(pack_element<index>(std::declval<type_container<Tp>>()...))
            >::type;

            template <typename... Tp>
            struct t_array {
                constexpr static size_t length = sizeof...(Tp);

                template<size_t index>
                using get = pack_element_t<index, Tp...>;

                template<typename T>
                using contains = micro_std::disjunction<std::bool_constant<std::is_same_v<T, Tp>>...>;

                template<typename T>
                using index_of = std::integral_constant<signed_size_t,
                c_array<bool, std::is_same_v<T, Tp>...>::index_of(true)>;

                template<template<typename...> typename X>
                using transform_all = X<Tp...>;

                template<typename V, template<typename> typename X>
                using transform_c = c_array<V, X<Tp>::value...>;

                template<template<typename> typename X>
                using transform_t = t_array<X<Tp>...>;
            };

            template<typename Arr, template<typename...> typename X>
            using t_transform_all = typename Arr::template transform_all<X>;

            template<typename Arr, typename V, template<typename> typename X>
            using t_transform_c = typename Arr::template transform_c<V, X>;

            template<typename Arr, template<typename> typename X>
            using t_transform_t = typename Arr::template transform_t<X>;

            template <typename T1, typename T2>
            using t_pair = t_array<T1, T2>;

            template<typename TArr, typename BArr, size_t low, size_t high>
            constexpr inline auto conditional_array_h() noexcept {
                static_assert(!(high < low), "high < low");
                if constexpr (high == low) {
                    return t_array<>();
                }
                constexpr size_t d = high - low;
                if constexpr (d == 1) {
                    return std::conditional_t < BArr::get(low),
                            t_array<typename TArr::template get < low>>, t_array<>>();
                }
                return decltype(append(
                        conditional_array_h<TArr, BArr, low, low + d / 2 > (),
                        conditional_array_h<TArr, BArr, low + d / 2, low + d > ()
                        ))();
            }

            //logarithmic algorithm
            template<typename TArr, typename BArr>
            using conditional_array = decltype(conditional_array_h<TArr, BArr, 0, TArr::length>());

            template<typename Arr>
            struct index_in_sorted_array_h {

                constexpr static size_t impl(size_t index) noexcept {
                    size_t out = 0;
                    for (size_t i = 0; i < index; i++) {
                        if (Arr::get(i) <= Arr::get(index)) {
                            out++;
                        }
                    }
                    for (size_t i = index + 1; i < Arr::length; i++) {
                        if (Arr::get(i) < Arr::get(index)) {
                            out++;
                        }
                    }
                    return out;
                }

                template<size_t index>
                using impl_t = std::integral_constant<size_t, impl(index)>;
            };

            template<typename Arr>
            using index_in_sorted_array = c_transform_c_auto<
            make_index_seq<0, Arr::length>, size_t,
            index_in_sorted_array_h<Arr>::template impl_t>;

            template<typename Arr>
            using index_to_sort_array = c_transform_c_auto<
            make_index_seq<0, Arr::length>, size_t,
            index_in_sorted_array<Arr>::template index_of_constant>;

            template<typename Arr>
            using sort_c_array = c_transform_c_auto<
            index_to_sort_array<Arr>,
            typename Arr::value_type, Arr::template get_constant>;

            template<typename Arr, typename V, template<typename> typename X>
            using sort_t_array = c_transform_t_auto<
            index_to_sort_array<t_transform_c<Arr, V, X>>,
            Arr::template get>;

            template<typename Arr>
            using sort_t_array_by_size = sort_t_array<Arr, size_t, size_of>;
        }

        namespace type_traits {

            //base settings
            using native_ints_t = seq::t_array<char, short, int, long, long long>;
            using extra_su_int_pairs_t = seq::t_array<
#ifdef __SIZEOF_INT128__
                    seq::t_pair<__int128, unsigned __int128>
#endif
                    >;

            template<size_t index>
            using extra_su_int_pair_by_index_t =
            typename extra_su_int_pairs_t::template get<index>;

            template<size_t index>
            using extra_s_int_by_index_t =
            typename extra_su_int_pair_by_index_t<index>::template get<0>;

            template<size_t index>
            using extra_u_int_by_index_t =
            typename extra_su_int_pair_by_index_t<index>::template get<1>;

            using extra_s_ints_t = seq::c_transform_t_auto<
                    seq::make_index_seq<0, extra_su_int_pairs_t::length>,
                    extra_s_int_by_index_t>;
            using extra_u_ints_t = seq::c_transform_t_auto<
                    seq::make_index_seq<0, extra_su_int_pairs_t::length>,
                    extra_u_int_by_index_t>;

            template<typename Arr>
            struct is_unique_h {

                constexpr static bool impl(size_t index) noexcept {
                    bool u = true;
                    for (size_t i = 0; i < index; i++) {
                        if (Arr::get(i) == Arr::get(index)) {
                            u = false;
                            break;
                        }
                    }
                    return u;
                }

                template<size_t index>
                using impl_t = std::bool_constant<impl(index)>;
            };

            template<typename Arr>
            using is_unique_array = seq::c_transform_c_auto<
            seq::make_index_seq<0, Arr::length>, bool,
            is_unique_h<Arr>::template impl_t>;

            template<typename Arr>
            using sizes_array = seq::t_transform_c<Arr, size_t, size_of>;

            template<typename Arr>
            using is_unique_size_array = is_unique_array<sizes_array<Arr>>;

            template<typename Arr>
            using make_unique_size_array = seq::conditional_array<Arr, is_unique_size_array<Arr>>;

            template<typename T, size_t length>
            using array_ref = T(&)[length];

            template<typename T, bool c, typename TT = micro_std::remove_const_t<T>>
            using add_const = std::conditional_t<c, micro_std::add_const_t<TT>, TT>;

            template<typename T, bool v, typename TT = micro_std::remove_volatile_t<T>>
            using add_volatile = std::conditional_t<v, micro_std::add_volatile_t<TT>, TT>;

            template<typename T, bool c, bool v>
            using add_cv = add_volatile<add_const<T, c>, v>;

            template<typename To, typename From>
            using share_cv = add_cv<To, std::is_const_v<From>, std::is_volatile_v<From>>;

            template<typename T1, typename T2>
            using is_same_ignore_cv = std::is_same<std::remove_cv_t<T1>, std::remove_cv_t<T2>>;

            template<typename T>
            using is_native_int = micro_std::conjunction<std::is_integral<T>, micro_std::negation<is_same_ignore_cv<T, bool>>>;

            template<typename T>
            using is_extra_int = micro_std::disjunction<
            typename extra_s_ints_t::contains<std::remove_cv_t<T>>,
            typename extra_u_ints_t::contains<std::remove_cv_t<T>>>;

            template<typename T>
            using is_integral = micro_std::disjunction<is_native_int<T>, is_extra_int<T>>;

            template<typename T>
            constexpr inline auto make_signed_h() {
                if constexpr (constexpr auto index = typename extra_s_ints_t::
                        template index_of<std::remove_cv_t < T >> (); index != -1) {
                    return seq::type_container<T> ();
                } else if constexpr (constexpr auto index =
                        typename extra_u_ints_t::
                        template index_of<std::remove_cv_t < T >> (); index != -1) {
                    return seq::type_container < share_cv<typename extra_s_ints_t::
                            template get < instantiation_context<T>(index) >, T >> ();
                } else if constexpr (is_native_int<T>()) {
                    return std::make_signed<T>();
                }
            }

            template<typename T>
            using make_signed = typename decltype(make_signed_h<T>())::type;

            template<typename T>
            constexpr inline auto make_unsigned_h() {
                if constexpr(constexpr auto index = typename extra_u_ints_t::
                        template index_of<std::remove_cv_t < T >> (); index != -1) {
                    return seq::type_container<T> ();
                } else if constexpr (constexpr auto index =
                        typename extra_s_ints_t::
                        template index_of<std::remove_cv_t < T >> (); index != -1) {
                    return seq::type_container < share_cv<typename extra_u_ints_t::
                            template get < instantiation_context<T>(index) >, T >> ();
                } else if constexpr (is_native_int<T>()) {
                    return std::make_unsigned<T>();
                }
            }

            template<typename T>
            using make_unsigned = typename decltype(make_unsigned_h<T>())::type;

            template<typename Arr>
            using make_signed_array = seq::t_transform_t<Arr, make_signed>;

            template<typename Arr>
            using make_unsigned_array = seq::t_transform_t<Arr, make_unsigned>;

            template<typename T, typename UT = make_unsigned<T>>
            constexpr inline size_t get_bits() {
                UT test = ~UT(0);
                size_t out = 0;
                while (test) {
                    test >>= 1;
                    out++;
                }
                return out;
            }

            using filtred_ints_t = seq::sort_t_array_by_size<
                    make_unique_size_array<seq::append_t<
                    make_unsigned_array<native_ints_t>, extra_u_ints_t>>>;

            using min_native_t = filtred_ints_t::get<0>;
            using max_native_t = filtred_ints_t::get<filtred_ints_t::length - 1 >;

            constexpr inline size_t min_native_size = sizeof (min_native_t);
            constexpr inline size_t max_native_size = sizeof (max_native_t);

            constexpr inline size_t max_native_bits = get_bits<max_native_t>();
            constexpr inline size_t min_native_bits = get_bits<min_native_t>();

            static_assert(min_native_size == 1, "min_native_size != 1");
            static_assert(min_native_bits == get_bits<char>(), "min_native_bits != char_bits");

            template<size_t... index>
            constexpr inline bool check_bits(seq::index_seq<index...>) {
                return micro_std::conjunction < std::bool_constant < (
                        get_bits<filtred_ints_t::get < index >> () ==
                        sizeof (filtred_ints_t::get < index >) * min_native_bits
                        )>...>();
            }

            static_assert(check_bits(seq::make_index_seq<0, filtred_ints_t::length>()), "check_bits failed");

            template<size_t bytes>
            using byte_to_bits = std::integral_constant<size_t, bytes * min_native_bits>;

            template<typename Arr>
            using bytes_to_bits = seq::c_transform_c_auto<Arr, size_t, byte_to_bits>;

            using int_sizes_t = sizes_array<filtred_ints_t>;
            using int_bits_t = bytes_to_bits<int_sizes_t>;

            template<size_t index, bool sig, typename I = typename filtred_ints_t::template get<index>>
            using int_of_index = std::conditional_t<sig, make_signed<I>, make_unsigned<I>>;

            template<size_t size, bool sig>
            using int_of_size = int_of_index<int_sizes_t::index_of(size), sig>;

            template<size_t bits, bool sig>
            using int_of_bits = int_of_index<int_bits_t::index_of(bits), sig>;

            template<typename T, typename U = make_unsigned<T>>
            constexpr inline bool is_pow2(T value) {
                U tmp = value;
                return tmp & (tmp - 1);
            }
        }

        namespace utils {

            constexpr inline seq::v_array<char, 62> digits = {
                //10
                '0', '1', '2', '3', '4',
                '5', '6', '7', '8', '9',
                //26
                'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'g', 'k', 'l', 'm',
                'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                //26
                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'G', 'K', 'L', 'M',
                'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
            };

            constexpr inline size_t digit_to_number(char c) noexcept {
                auto index = digits.index_of(c);
                return index < 36 ? index : index - 26;
            }

            template<typename T, typename U = type_traits::make_unsigned<T>>
            constexpr inline int popcount(T tmp) noexcept {
                using namespace type_traits;
                U value = tmp;
#if __has_builtin(__builtin_popcount)
                if constexpr (sizeof (U) == sizeof (unsigned int)) {
                    return __builtin_popcount(value);
                } else
#endif
#if __has_builtin(__builtin_popcountl)
                    if constexpr (sizeof (U) == sizeof (unsigned long)) {
                    return __builtin_popcountl(value);
                } else
#endif
#if __has_builtin(__builtin_popcountll)
                    if constexpr (sizeof (U) == sizeof (unsigned long long)) {
                    return __builtin_popcountll(value);
                } else
#endif
                    if constexpr ((get_bits<U>() == 128) && int_bits_t::contains(64)) {
                    using u64 = int_of_bits < instantiation_context<T>(64), false >;
                    return popcount<u64>(value) + popcount<u64>(value >> 64);
                } else if constexpr (get_bits<U>() == 64) {
                    value -= ((value >> 1) & 0x5555555555555555U);
                    value = ((value >> 2) & 0x3333333333333333U) + (value & 0x3333333333333333U);
                    return ((((value >> 4) + value) & 0xf0f0f0f0f0f0f0fU) * 0x101010101010101U) >> 56;
                } else if constexpr (get_bits<U>() == 32) {
                    value -= ((value >> 1) & 0x55555555U);
                    value = ((value >> 2) & 0x33333333U) + (value & 0x33333333U);
                    return ((((value >> 4) + value) & 0xf0f0f0fU) * 0x1010101U) >> 24;
                } else if constexpr (get_bits<U>() < get_bits<unsigned int>()) {
                    return popcount<unsigned int>(value);
                } else if constexpr (get_bits<U>() < get_bits<unsigned long>()) {
                    return popcount<unsigned long>(value);
                } else if constexpr (get_bits<U>() < get_bits<unsigned long long>()) {
                    return popcount<unsigned long long>(value);
                } else if constexpr (int_bits_t::contains(128) && (get_bits<U>() < 128)) {
                    using u128 = int_of_bits < instantiation_context<T>(128), false >;
                    return popcount<u128>(value);
                } else {
                    int out = 0;
                    while (value) {
                        value = (value - 1) & value;
                        out++;
                    }
                    return out;
                }
            }

            constexpr inline size_t lpo2_s(size_t value) noexcept {
                size_t i = 0;
                size_t p = 1;
                for (; p < value && i < type_traits::get_bits<size_t>(); i++) {
                    p <<= 1;
                }
                return i - 1;
            }

            template<size_t shr>
            using shr_one = std::integral_constant<size_t, size_t(1) << shr>;

            template<size_t bits, typename Arr = seq::c_transform_c_auto<seq::make_index_seq < 0, lpo2_s(bits)>, size_t, shr_one>>
            using sh_array = seq::append_t<Arr, seq::index_seq<bits - 2U * Arr::get(Arr::length - 1U)>>;

            template<typename T, size_t... sh>
            constexpr inline T next_pow2_sub1_h(T value, seq::index_seq<sh...>) noexcept {
                seq::ignore_t{(value |= (value >> sh))...};
                return value;
            }

            template<typename T, typename U = type_traits::make_unsigned<T>>
            constexpr inline T next_pow2_sub1(T value) noexcept {
                return next_pow2_sub1_h<U>(value, sh_array < type_traits::get_bits<T>()>());
            }

            template<typename T, typename U = type_traits::make_unsigned<T>,
            size_t bits = type_traits::get_bits<T>()>
            constexpr inline int clz(T tmp) noexcept {
                using namespace type_traits;
                U value = tmp;
#if __has_builtin(__builtin_clz)
                if constexpr (sizeof (U) == sizeof (unsigned int)) {
                    return value == 0 ? bits : __builtin_clz(value);
                } else
#endif
#if __has_builtin(__builtin_clzl)
                    if constexpr (sizeof (U) == sizeof (unsigned long)) {
                    return value == 0 ? bits : __builtin_clzl(value);
                } else
#endif
#if __has_builtin(__builtin_clzll)
                    if constexpr (sizeof (U) == sizeof (unsigned long long)) {
                    return value == 0 ? bits : __builtin_clzll(value);
                } else
#endif
                    if constexpr ((get_bits<U>() == 128) && int_bits_t::contains(64)) {
                    using u64 = int_of_bits < instantiation_context<T>(64), false >;
                    u64 high = value >> 64;
                    return high == 0 ? 64 + clz<u64>(value) : clz<u64>(high);
                } else if constexpr (get_bits<U>() < get_bits<unsigned int>()) {
                    return clz<unsigned int>(value);
                } else if constexpr (get_bits<U>() < get_bits<unsigned long>()) {
                    return clz<unsigned long>(value);
                } else if constexpr (get_bits<U>() < get_bits<unsigned long long>()) {
                    return clz<unsigned long long>(value);
                } else if constexpr (int_bits_t::contains(128) && (get_bits<U>() < 128)) {
                    using u128 = int_of_bits < instantiation_context<T>(128), false >;
                    return clz<u128>(value);
                } else {
                    return bits - popcount<U>(next_pow2_sub1<U>(value));
                }
            }

            template<typename T, typename U = type_traits::make_unsigned<T>,
            size_t bits = type_traits::get_bits<T>()>
            constexpr inline int ctz(T tmp) noexcept {
                using namespace type_traits;
                U value = tmp;
#if __has_builtin(__builtin_ctz)
                if constexpr (sizeof (U) == sizeof (unsigned int)) {
                    return value == 0 ? bits : __builtin_ctz(value);
                } else
#endif
#if __has_builtin(__builtin_ctzl)
                    if constexpr (sizeof (U) == sizeof (unsigned long)) {
                    return value == 0 ? bits : __builtin_ctzl(value);
                } else
#endif
#if __has_builtin(__builtin_ctzll)
                    if constexpr (sizeof (U) == sizeof (unsigned long long)) {
                    return value == 0 ? bits : __builtin_ctzll(value);
                } else
#endif
                    if constexpr ((get_bits<U>() == 128) && int_bits_t::contains(64)) {
                    using u64 = int_of_bits < instantiation_context<T>(64), false >;
                    u64 low = value;
                    return low == 0 ? 64 + ctz<u64>(value >> 64) : ctz<u64>(low);
                } else if constexpr (get_bits<U>() < get_bits<unsigned int>()) {
                    return ctz<unsigned int>(value);
                } else if constexpr (get_bits<U>() < get_bits<unsigned long>()) {
                    return ctz<unsigned long>(value);
                } else if constexpr (get_bits<U>() < get_bits<unsigned long long>()) {
                    return ctz<unsigned long long>(value);
                } else if constexpr (int_bits_t::contains(128) && (get_bits<U>() < 128)) {
                    using u128 = int_of_bits < instantiation_context<T>(128), false >;
                    return ctz<u128>(value);
                } else {
                    return popcount<U>(U(value & U(-value)) - U(1));
                }
            }
        }

        namespace impl {

            enum IKind {
                illegal = 0, native, pow2, array
            };

            constexpr inline IKind i_kind(size_t size) noexcept {
                if (!size) {
                    return illegal;
                }
                if (type_traits::int_sizes_t::contains(size)) {
                    return native;
                }
                return (size % type_traits::max_native_size == 0) &&
                        type_traits::is_pow2(size / type_traits::max_native_size) ?
                        pow2 : array;
            }

            template<size_t size, bool sig, bool le, IKind = i_kind(size)>
            struct integer_impl;

            template<size_t size, bool sig, bool le>
            struct native_integer_base;

            template<size_t size, bool sig, bool le>
            struct native_integer_impl;

            template<size_t size, bool sig, bool le>
            struct integer_impl <size, sig, le, native> {
                using type = native_integer_impl<size, sig, le>;
            };

            template<size_t size, bool le>
            struct native_integer_base<size, false, le> {
                using S = type_traits::int_of_size<size, true>;
                using U = type_traits::int_of_size<size, false>;
                using I = native_integer_base;
                using M = size_t; //typedef p_SHType<sizeof (U) > M;
                constexpr static M max_sh = size * type_traits::min_native_bits - 1;
                U value;

                constexpr native_integer_base() noexcept = default;

                constexpr explicit native_integer_base(U n) noexcept : value(n) { }

                constexpr bool is_negative() const noexcept {
                    return false;
                }

                constexpr static bool increment_overflow(I &out) noexcept {
                    return !(++out.value);
                }

                constexpr static bool decrement_overflow(I &out) noexcept {
                    return !(out.value--);
                }

                constexpr static bool increment_carry(I &out, bool cf) noexcept {
                    return (out.value += 1 + cf) <= cf;
                }

                constexpr static bool decrement_carry(I &out, bool cf) noexcept {
                    bool tmp = out.value <= cf;
                    out.value -= 1 + cf;
                    return tmp;
                }

                constexpr static bool add_overflow(I v1, I v2, I &out) noexcept {
                    return (out.value = v1.value + v2.value) < v1.value;
                }

                constexpr static bool sub_overflow(I v1, I v2, I &out) noexcept {
                    return (out.value = v1.value - v2.value) > v1.value;
                }

                constexpr static bool
                add_carry(I v1, I v2, bool cf, I &out) noexcept {
                    bool tmp = add_overflow(v1, v2, out);
                    return add_overflow(out, I(cf), out) | tmp;
                }

                constexpr static bool
                sub_carry(I v1, I v2, bool cf, I &out) noexcept {
                    bool tmp = sub_overflow(v1, v2, out);
                    return sub_overflow(out, I(cf), out) | tmp;
                }

                constexpr static bool
                add_zero_carry(I v1, bool cf, I &out) noexcept {
                    return add_overflow(v1, I(cf), out);
                }

                constexpr static bool
                sub_zero_carry(I v1, bool cf, I &out) noexcept {
                    return sub_overflow(v1, I(cf), out);
                }

                constexpr I operator/(I other) const noexcept {
                    return I(value / other.value);
                }

                constexpr I operator%(I other) const noexcept {
                    return I(value % other.value);
                }

                constexpr I operator>>(M other) const noexcept {
                    return I(other > max_sh ? 0 : value >> other);
                }

                constexpr bool operator>(I other) const noexcept {
                    return value > other.value;
                }

                constexpr bool operator<(I other) const noexcept {
                    return value < other.value;
                }

                constexpr bool operator>=(I other) const noexcept {
                    return value >= other.value;
                }

                constexpr bool operator<=(I other) const noexcept {
                    return value <= other.value;
                }
            };

            template<size_t size, bool sig, bool le>
            struct native_integer_impl : public native_integer_base<size, sig, le> {
                using B = native_integer_base<size, sig, le>;
                using I = native_integer_impl<size, sig, le>;
                using UI = native_integer_impl<size, false, le>;
                using SI = native_integer_impl<size, true, le>;
                using U = typename B::U;
                using S = typename B::S;
                using M = typename B::M;

                using B::B;
                using B::value;

                constexpr native_integer_impl() noexcept = default;

                constexpr native_integer_impl(B obj) noexcept : B(obj) { }

                constexpr static I ZERO() noexcept {
                    return B(0);
                }

                constexpr bool is_zero() const noexcept {
                    return value == 0;
                }

                constexpr bool upper_bit() const noexcept {
                    return value >> (size * type_traits::min_native_bits - 1);
                }

                constexpr I add_one() const noexcept {
                    return I(value + 1);
                }

                constexpr I sub_one() const noexcept {
                    return I(value - 1);
                }

                template<size_t index>
                constexpr type_traits::min_native_t getByte() const noexcept {
                    return value >> (index * type_traits::min_native_bits);
                }

                template<size_t index, U mask = ~(U(type_traits::min_native_t(~0U)) <<
                        (index * type_traits::min_native_bits))>
                constexpr void setByte(type_traits::min_native_t v) noexcept {
                    value = (value & mask) | (U(v) << (index * type_traits::min_native_bits));
                }

                template<size_t index, U mask = U(1) << index>
                constexpr bool getBit() const noexcept {
                    return value & mask;
                }

                template<size_t index, U mask1 = U(1) << index, U mask2 = ~mask1>
                constexpr void setBit(bool v) noexcept {
                    value = v ? (value | mask1) : (value & mask2);
                }

                constexpr static bool increment_overflow(I &out) noexcept {
                    return B::increment_overflow(out);
                }

                constexpr static bool decrement_overflow(I &out) noexcept {
                    return B::decrement_overflow(out);
                }

                constexpr static bool
                increment_carry(I &out, bool cf) noexcept {
                    return B::increment_carry(out, cf);
                }

                constexpr static bool
                decrement_carry(I &out, bool cf) noexcept {
                    return B::decrement_carry(out, cf);
                }

                constexpr static bool
                add_overflow(I v1, I v2, I &out) noexcept {
                    return B::add_overflow(v1, v2, out);
                }

                constexpr static bool
                sub_overflow(I v1, I v2, I &out) noexcept {
                    return B::sub_overflow(v1, v2, out);
                }

                constexpr static bool
                add_carry(I v1, I v2, bool cf, I &out) noexcept {
                    return B::add_carry(v1, v2, cf, out);
                }

                constexpr static bool
                sub_carry(I v1, I v2, bool cf, I &out) noexcept {
                    return B::sub_carry(v1, v2, cf, out);
                }

                constexpr static bool
                add_zero_carry(I v1, bool cf, I &out) noexcept {
                    return B::add_zero_carry(v1, cf, out);
                }

                constexpr static bool
                sub_zero_carry(I v1, bool cf, I &out) noexcept {
                    return B::sub_zero_carry(v1, cf, out);
                }

                constexpr static bool
                left_shift_one_bit(I &out, bool bit) noexcept {
                    bool tmp = out.upper_bit();
                    out.value = (out.value << 1) | U(bit);
                    return tmp;
                }

                constexpr size_t clz() const noexcept {
                    return utils::clz(value);
                }

                constexpr size_t ctz() const noexcept {
                    return utils::ctz(value);
                }

                constexpr I operator+() const noexcept {
                    return *this;
                }

                constexpr I operator-() const noexcept {
                    return I(-value);
                }

                constexpr I operator+(I other) const noexcept {
                    return I(value + other.value);
                }

                constexpr I operator-(I other) const noexcept {
                    return I(value - other.value);
                }

                constexpr I operator*(I other) const noexcept {
                    return I(value * other.value);
                }

                constexpr I operator/(I other) const noexcept {
                    return I(this->B::operator/(other));
                }

                constexpr I operator|(I other) const noexcept {
                    return I(value | other.value);
                }

                constexpr I operator&(I other) const noexcept {
                    return I(value & other.value);
                }

                constexpr I operator^(I other) const noexcept {
                    return I(value ^ other.value);
                }

                constexpr I operator<<(M other) const noexcept {
                    return I(other > B::max_sh ? 0 : value >> other);
                }

                constexpr I operator>>(M other) const noexcept {
                    return I(this->B::operator>>(other));
                }

                constexpr I operator~() const noexcept {
                    return I(~value);
                }

                constexpr bool operator==(I other) const noexcept {
                    return value == other.value;
                }

                constexpr bool operator!=(I other) const noexcept {
                    return value != other.value;
                }

                constexpr bool operator>(I other) const noexcept {
                    return this->B::operator>(other);
                }

                constexpr bool operator<(I other) const noexcept {
                    return this->B::operator<(other);
                }

                constexpr bool operator>=(I other) const noexcept {
                    return this->B::operator>=(other);
                }

                constexpr bool operator<=(I other) const noexcept {
                    return this->B::operator<=(other);
                }
            };
        }
    }

    template<size_t size, bool sig, bool le = true >
    class integer;
}

#undef INTEGER_HPP_HAS_OSTREAM

#ifndef INTEGER_HPP_HAS_BUILTIN_CHECK
#undef __has_builtin
#else
#undef INTEGER_HPP_HAS_BUILTIN_CHECK
#endif

#endif /* INTEGER_HPP */

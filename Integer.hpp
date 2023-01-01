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

#if __has_include(<limits>)
#include <limits>
#define INTEGER_HPP_HAS_LIMITS
#endif

#if __has_include(<bit>)
#include <bit>
#define INTEGER_HPP_HAS_BIT
#endif

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

        template<size_t value>
        using size_constant = std::integral_constant<size_t, value>;

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

            template<typename T>
            constexpr inline const T& min(const T &a, const T &b) {
                return a < b ? a : b;
            }

            template<typename T>
            constexpr inline const T& max(const T &a, const T &b) {
                return a > b ? a : b;
            }
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

            template<typename Arr, size_t index>
            using get_t = typename Arr::template get_t<index>;

            template <typename T, T... values>
            struct c_array {
                using value_type = T;
                constexpr static size_t length = sizeof...(values);

                constexpr static T get(size_t index) noexcept {
                    constexpr T data[length == 0 ? 1 : length] = {values...};
                    return data[index];
                }

                template<size_t index>
                using get_t = std::integral_constant<T, get(index)>;

                template<template<typename P, P...> typename X>
                using transform_all = X<T, values...>;

                template<template<auto...> typename X>
                using transform_all_auto = X<values...>;

                template<typename V, template<typename P, P...> typename X>
                using transform_c = c_array<V, X<T, values>::value...>;

                template<typename V, template<auto...> typename X>
                using transform_c_auto = c_array<V, X<values>::value...>;

                template<template<typename P, P...> typename X>
                using transform_t = t_array<X<T, values>...>;

                template<template<auto...> typename X>
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
                using index_of_t = std::integral_constant<size_t, index_of(value)>;

                constexpr inline static bool contains(T value) noexcept {
                    return index_of(value) != -1;
                }

                template<T value>
                using contains_t = std::bool_constant<contains(value)>;
            };

            template <size_t... values>
            using index_seq = c_array<size_t, values...>;

            template<typename Arr, template<typename P, P...> typename X>
            using c_transform_all = typename Arr::template transform_all<X>;

            template<typename Arr, template<auto...> typename X>
            using c_transform_all_auto = typename Arr::template transform_all_auto<X>;

            template<typename Arr, typename V, template<typename P, P...> typename X>
            using c_transform_c = typename Arr::template transform_c<V, X>;

            template<typename Arr, typename V, template<auto...> typename X>
            using c_transform_c_auto = typename Arr::template transform_c_auto<V, X>;

            template<typename Arr, template<typename P, P...> typename X>
            using c_transform_t = typename Arr::template transform_t<X>;

            template<typename Arr, template<auto...> typename X>
            using c_transform_t_auto = typename Arr::template transform_t_auto<X>;

            template <typename CArr>
            using c_to_t = c_transform_t<CArr, std::template integral_constant>;

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
                } else {
                    constexpr T d = high - low;
                    if constexpr (d == 1) {
                        return c_array<T, low>();
                    } else {
                        return append(
                                make_seq_h<T, low, low + d / 2 > (),
                                make_seq_h<T, low + d / 2, low + d > ()
                                );
                    }
                }
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
                    return micro_std::forward<T>(out);
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
            decltype(pack_element<index>(type_container<Tp>()...))>::type;

            template <typename... Tp>
            struct t_array {
                constexpr static size_t length = sizeof...(Tp);

                template<size_t index>
                using get_t = pack_element_t<index, Tp...>;

                template<typename T>
                using contains_t = micro_std::disjunction<std::bool_constant<std::is_same_v<T, Tp>>...>;

                template<typename T>
                using index_of_t = std::integral_constant<signed_size_t,
                c_array<bool, std::is_same_v<T, Tp>...>::index_of(true)>;

                template<template<typename...> typename X>
                using transform_all = X<Tp...>;

                template<typename V, template<typename...> typename X>
                using transform_c = c_array<V, X<Tp>::value...>;

                template<template<typename...> typename X>
                using transform_t = t_array<X<Tp>...>;
            };

            template <typename T1, typename T2>
            using t_pair = t_array<T1, T2>;

            template<typename Arr, template<typename...> typename X>
            using t_transform_all = typename Arr::template transform_all<X>;

            template<typename Arr, typename V, template<typename...> typename X>
            using t_transform_c = typename Arr::template transform_c<V, X>;

            template<typename Arr, template<typename...> typename X>
            using t_transform_t = typename Arr::template transform_t<X>;

            template<typename T>
            using id_type = T;

            template <typename TArr, typename V>
            using t_to_c = t_transform_c<TArr, V, id_type>;

            namespace array_ops_h {

                template<size_t from, size_t to>
                struct remove_index {
                    template<size_t index>
                    using impl = size_constant<(index < from) ? index : to + index - from>;
                };

                template<size_t length, size_t from, size_t to>
                using remove_index_t = c_transform_c_auto<
                seq::make_index_seq < 0, length - to + from>, size_t,
                remove_index<from, to>::template impl>;

                template<typename Arr1, size_t index1, typename Arr2, size_t index2, size_t length>
                struct insert {

                    template<size_t index>
                    constexpr static auto impl_f() noexcept {
                        if constexpr (index < index1) {
                            return get_t<Arr1, index>();
                        } else if constexpr (index < index1 + length) {
                            return get_t<Arr2, index - index1 + index2 > ();
                        } else {
                            return get_t<Arr1, index - length > ();
                        }
                    }

                    template<size_t index>
                    using impl = decltype(impl_f<index>());
                };

                template<typename Arr1, size_t index1, typename Arr2, size_t index2, size_t length>
                struct replace {

                    template<size_t index>
                    constexpr static auto impl_f() noexcept {
                        if constexpr (index < index1) {
                            return get_t<Arr1, index>();
                        } else if constexpr (index < index1 + length) {
                            return get_t<Arr2, index - index1 + index2 > ();
                        } else {
                            return get_t<Arr1, index> ();
                        }
                    }

                    template<size_t index>
                    using impl = decltype(impl_f<index>());
                };
            }

            template<typename Arr, size_t from, size_t to>
            using t_remove_from_to = c_transform_t_auto<
            array_ops_h::remove_index_t<Arr::length, from, to>,
            Arr::template get_t>;

            template<typename Arr, size_t from, size_t length>
            using t_remove_from_length = t_remove_from_to<Arr, from, from + length>;

            template<typename Arr, size_t index>
            using t_remove_single = t_remove_from_length<Arr, index, 1>;

            template<typename Arr, size_t from, size_t to>
            using c_remove_from_to = c_transform_c_auto<
            array_ops_h::remove_index_t<Arr::length, from, to>,
            typename Arr::value_type, Arr::template get_t>;

            template<typename Arr, size_t from, size_t length>
            using c_remove_from_length = c_remove_from_to<Arr, from, from + length>;

            template<typename Arr, size_t index>
            using c_remove_single = c_remove_from_length<Arr, index, 1>;

            template<typename Arr1, size_t index1, typename Arr2, size_t index2, size_t length>
            using t_insert = c_transform_t_auto<seq::make_index_seq < 0, Arr1::length + length>,
            array_ops_h::insert<Arr1, index1, Arr2, index2, length>::template impl>;

            template<typename Arr1, size_t index, typename Arr2>
            using t_insert_fully = t_insert<Arr1, index, Arr2, 0, Arr2::length>;

            template<typename Arr, size_t index, typename... Tp>
            using t_insert_pack = t_insert_fully<Arr, index, t_array<Tp...>>;

            template<typename Arr1, size_t index1, typename Arr2, size_t index2, size_t length>
            using c_insert = c_transform_c_auto<seq::make_index_seq < 0, Arr1::length + length>,
            typename Arr1::value_type,
            array_ops_h::insert<Arr1, index1, Arr2, index2, length>::template impl>;

            template<typename Arr1, size_t index, typename Arr2>
            using c_insert_fully = c_insert<Arr1, index, Arr2, 0, Arr2::length>;

            template<typename Arr, size_t index, typename Arr::value_type... v>
            using c_insert_pack = c_insert_fully<Arr, index, c_array<typename Arr::value_type, v...>>;

            template<typename Arr1, size_t index1, typename Arr2, size_t index2, size_t length>
            using t_replace = c_transform_t_auto<seq::make_index_seq < 0,
            micro_std::max(Arr1::length, length + index1)>,
            array_ops_h::replace<Arr1, index1, Arr2, index2, length>::template impl>;

            template<typename Arr1, size_t index, typename Arr2>
            using t_replace_fully = t_replace<Arr1, index, Arr2, 0, Arr2::length>;

            template<typename Arr, size_t index, typename... Tp>
            using t_replace_pack = t_replace_fully<Arr, index, t_array<Tp...>>;

            template<typename Arr1, size_t index1, typename Arr2, size_t index2, size_t length>
            using c_replace = c_transform_c_auto<seq::make_index_seq < 0,
            micro_std::max(Arr1::length, length + index1)>,
            typename Arr1::value_type,
            array_ops_h::replace<Arr1, index1, Arr2, index2, length>::template impl>;

            template<typename Arr1, size_t index, typename Arr2>
            using c_replace_fully = c_replace<Arr1, index, Arr2, 0, Arr2::length>;

            template<typename Arr, size_t index, typename Arr::value_type... v>
            using c_replace_pack = c_replace_fully<Arr, index, c_array<typename Arr::value_type, v...>>;

            template<typename TArr, typename BArr, size_t low, size_t high>
            constexpr inline auto conditional_array_h() noexcept {
                static_assert(!(high < low), "high < low");
                if constexpr (high == low) {
                    return t_array<>();
                } else {
                    constexpr size_t d = high - low;
                    if constexpr (d == 1) {
                        return std::conditional_t < BArr::get(low),
                                t_array<get_t<TArr, low>>, t_array<>>();
                    } else {
                        return append(
                                conditional_array_h<TArr, BArr, low, low + d / 2 > (),
                                conditional_array_h<TArr, BArr, low + d / 2, low + d > ()
                                );
                    }
                }
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
            index_in_sorted_array<Arr>::template index_of_t>;

            template<typename Arr>
            using sort_c_array = c_transform_c_auto<
            index_to_sort_array<Arr>,
            typename Arr::value_type, Arr::template get_t>;

            template<typename Arr, typename V, template<typename...> typename X>
            using sort_t_array = c_transform_t_auto<
            index_to_sort_array<t_transform_c<Arr, V, X>>,
            Arr::template get_t>;

            template<typename Arr>
            using sort_t_array_by_size = sort_t_array<Arr, size_t, size_of>;

            template<typename data, typename LeavesArr>
            struct a_tree {
                using type = data;
                constexpr static size_t length = LeavesArr::length;

                template<typename Arr, size_t id = 0 >
                constexpr static auto get_h() noexcept {
                    if constexpr (Arr::length == id) {
                        return a_tree();
                    } else {
                        return get_t<LeavesArr, Arr::get(id)>::template get_h<Arr, id + 1 > ();
                    }
                }

                template<typename Arr>
                using a_get_leaf = decltype(get_h<Arr>());

                template<size_t... arr>
                using get_leaf = a_get_leaf<index_seq<arr...>>;

                template<typename Arr>
                using a_get = typename a_get_leaf<Arr>::type;

                template<size_t... arr>
                using get = a_get<index_seq<arr...>>;

                template<typename Arr, size_t id = 0 >
                constexpr static auto remove_h() noexcept {
                    if constexpr (Arr::length - 1 == id) {
                        return a_tree<data, t_remove_single<LeavesArr, Arr::get(id)>>();
                    } else {
                        return get_t<LeavesArr, Arr::get(id)>::template remove_h<Arr, id + 1 > ();
                    }
                }

                template<typename Arr>
                using a_remove_leaf = decltype(remove_h<Arr>());

                template<size_t... arr>
                using remove_leaf = a_remove_leaf<index_seq<arr...>>;
            };

            template<typename data, typename... Leaves>
            using tree = a_tree<data, t_array<Leaves...>>;

            template<typename Tree, typename Arr>
            using a_tree_get = typename Tree::template a_get<Arr>;

            template<typename Tree, size_t... arr>
            using tree_get = a_tree_get<Tree, index_seq<arr...>>;

            template<typename Tree, typename Arr>
            using a_tree_get_leaf = typename Tree::template a_get_leaf<Arr>;

            template<typename Tree, size_t... arr>
            using tree_get_leaf = a_tree_get_leaf<Tree, index_seq<arr...>>;

            template<typename Tree, typename Arr>
            using a_tree_remove_leaf = typename Tree::template a_remove_leaf<Arr>;

            template<typename Tree, size_t... arr>
            using tree_remove_leaf = a_tree_get_leaf<Tree, index_seq<arr...>>;
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
            using extra_s_int_by_index_t =
            seq::get_t<seq::get_t<extra_su_int_pairs_t, index>, 0>;

            template<size_t index>
            using extra_u_int_by_index_t =
            seq::get_t<seq::get_t<extra_su_int_pairs_t, index>, 1>;

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
            using is_native_int = micro_std::conjunction<std::is_integral<T>,
            micro_std::negation<is_same_ignore_cv<T, bool>>>;

            template<typename T>
            using is_extra_int = micro_std::disjunction<
            typename extra_s_ints_t::template contains_t<std::remove_cv_t<T>>,
            typename extra_u_ints_t::template contains_t<std::remove_cv_t<T>>>;

            template<typename T>
            using is_integral = micro_std::disjunction<is_native_int<T>, is_extra_int<T>>;

            template<typename T>
            constexpr inline auto make_signed_h() {
                if constexpr (constexpr auto index = typename extra_s_ints_t::
                        template index_of_t<std::remove_cv_t < T >> (); index != -1) {
                    return seq::type_container<T> ();
                } else if constexpr (constexpr auto index =
                        typename extra_u_ints_t::
                        template index_of_t<std::remove_cv_t < T >> (); index != -1) {
                    return seq::type_container < share_cv <
                            seq::get_t <extra_s_ints_t, instantiation_context<T>(index) >, T >> ();
                } else if constexpr (is_native_int<T>()) {
                    return std::make_signed<T>();
                }
            }

            template<typename T>
            using make_signed = typename decltype(make_signed_h<T>())::type;

            template<typename T>
            constexpr inline auto make_unsigned_h() {
                if constexpr(constexpr auto index = typename extra_u_ints_t::
                        template index_of_t<std::remove_cv_t < T >> (); index != -1) {
                    return seq::type_container<T> ();
                } else if constexpr (constexpr auto index =
                        typename extra_s_ints_t::
                        template index_of_t<std::remove_cv_t < T >> (); index != -1) {
                    return seq::type_container < share_cv <
                            seq::get_t <extra_u_ints_t, instantiation_context<T>(index) >, T >> ();
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
#ifdef INTEGER_HPP_HAS_LIMITS
                if constexpr (std::numeric_limits<UT>::is_specialized) {
                    return std::numeric_limits<UT>::digits;
                } else
#endif
                {
                    UT test = ~UT(0);
                    size_t out = 0;
                    while (test) {
                        test >>= 1;
                        out++;
                    }
                    return out;
                }
            }

            constexpr inline size_t char_bits = get_bits<char>();
            constexpr inline size_t short_bits = get_bits<short>();
            constexpr inline size_t int_bits = get_bits<int>();
            constexpr inline size_t long_bits = get_bits<long>();
            constexpr inline size_t long_long_bits = get_bits<long long>();

            using filtred_ints_t = seq::sort_t_array_by_size<
                    make_unique_size_array<seq::append_t<
                    make_unsigned_array<native_ints_t>, extra_u_ints_t>>>;

            using min_native_t = seq::get_t<filtred_ints_t, 0>;
            using max_native_t = seq::get_t<filtred_ints_t, filtred_ints_t::length - 1 >;

            constexpr inline size_t min_native_size = sizeof (min_native_t);
            constexpr inline size_t max_native_size = sizeof (max_native_t);

            constexpr inline size_t max_native_bits = get_bits<max_native_t>();
            constexpr inline size_t min_native_bits = get_bits<min_native_t>();

            static_assert(min_native_size == 1, "min_native_size != 1");
            static_assert(min_native_bits == char_bits, "min_native_bits != char_bits");

            template<size_t... index>
            constexpr inline bool check_bits(seq::index_seq<index...>) {
                return micro_std::conjunction < std::bool_constant < (
                        get_bits<seq::get_t<filtred_ints_t, index >> () ==
                        sizeof (seq::get_t<filtred_ints_t, index>) * min_native_bits
                        )>...>();
            }

            static_assert(check_bits(seq::make_index_seq<0, filtred_ints_t::length>()), "check_bits failed");

            template<size_t bytes>
            using byte_to_bits = std::integral_constant<size_t, bytes * min_native_bits>;

            template<typename Arr>
            using bytes_to_bits = seq::c_transform_c_auto<Arr, size_t, byte_to_bits>;

            using int_sizes_t = sizes_array<filtred_ints_t>;
            using int_bits_t = bytes_to_bits<int_sizes_t>;

            template<size_t index, bool sig, typename I = seq::get_t<filtred_ints_t, index>>
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

            namespace endian_h {

                static_assert(size_t(min_native_t(~min_native_t(0))) >=
                        max_native_size, "too big max_native_size");

                constexpr inline max_native_t little() noexcept {
                    max_native_t out = 0;
                    for (size_t i = 0; i < max_native_size; i++) {
                        out <<= min_native_bits;
                        out |= min_native_t(max_native_size - i);
                    }
                    return out;
                }

                constexpr inline max_native_t big() noexcept {
                    max_native_t out = 0;
                    for (size_t i = 0; i < max_native_size; i++) {
                        out <<= min_native_bits;
                        out |= min_native_t(i + 1);
                    }
                    return out;
                }

                constexpr inline auto native() noexcept {
                    seq::v_array<min_native_t, max_native_size> out = {};
                    for (size_t i = 0; i < max_native_size; i++) {
                        out[i] = min_native_t(i + 1);
                    }
                    return out;
                }
            }

            enum struct endian {
                little = 0, big, mixed, undefined,
#if __cpp_lib_endian >= 201907L
                        native = (std::endian::native == std::endian::little) ? little :
                        ((std::endian::native == std::endian::big) ? big : mixed)
#elif __cpp_lib_bit_cast >= 201806L
                        native = (endian_h::little() == std::bit_cast<max_native_t>(endian_h::native())) ? little :
                        ((endian_h::big() == std::bit_cast<max_native_t>(endian_h::native())) ? big : mixed)
#elif __has_builtin(__builtin_bit_cast)
                        native = (endian_h::little() == __builtin_bit_cast(max_native_t, endian_h::native())) ? little :
                        ((endian_h::big() == __builtin_bit_cast(max_native_t, endian_h::native())) ? big : mixed)
#else
                        native = undefined
#endif
            };
        }

        namespace utils {

            template<size_t size>
            using sh_type = size_t;

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

            template<typename T, typename U = type_traits::make_unsigned<T>,
            size_t bits = type_traits::get_bits<T>()>
            constexpr inline int popcount(T tmp) noexcept {
                using namespace type_traits;
                U value = tmp;
#if __cpp_lib_bitops >= 201907L
                if constexpr (is_native_int<U>()) {
                    return std::popcount(value);
                } else
#endif
#if __has_builtin(__builtin_popcount)
                    if constexpr (bits == int_bits) {
                    return __builtin_popcount(value);
                } else
#endif
#if __has_builtin(__builtin_popcountl)
                    if constexpr (bits == long_bits) {
                    return __builtin_popcountl(value);
                } else
#endif
#if __has_builtin(__builtin_popcountll)
                    if constexpr (bits == long_long_bits) {
                    return __builtin_popcountll(value);
                } else
#endif
                    if constexpr ((bits == 128) && int_bits_t::contains(64)) {
                    using u64 = int_of_bits < instantiation_context<T>(64), false >;
                    return popcount<u64>(value) + popcount<u64>(value >> 64);
                } else if constexpr (bits == 64) {
                    value -= ((value >> 1) & 0x5555555555555555U);
                    value = ((value >> 2) & 0x3333333333333333U) + (value & 0x3333333333333333U);
                    return ((((value >> 4) + value) & 0xf0f0f0f0f0f0f0fU) * 0x101010101010101U) >> 56;
                } else if constexpr (bits == 32) {
                    value -= ((value >> 1) & 0x55555555U);
                    value = ((value >> 2) & 0x33333333U) + (value & 0x33333333U);
                    return ((((value >> 4) + value) & 0xf0f0f0fU) * 0x1010101U) >> 24;
                } else if constexpr ((bits == int_bits) || (bits == long_bits) ||
                        (bits == long_long_bits) || (bits >= 128) ||
                        ((bits > long_long_bits) && !int_bits_t::contains(128))) {
                    int out = 0;
                    while (value) {
                        value = U(value - 1) & value;
                        out++;
                    }
                    return out;
                } else if constexpr (bits < int_bits) {
                    return popcount<int>(value);
                } else if constexpr (bits < long_bits) {
                    return popcount<long>(value);
                } else if constexpr (bits < long_long_bits) {
                    return popcount<long long>(value);
                } else if constexpr ((bits < 128) && int_bits_t::contains(128)) {
                    using u128 = int_of_bits < instantiation_context<T>(128), false >;
                    return popcount<u128>(value);
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
#if __cpp_lib_bitops >= 201907L
                if constexpr (is_native_int<U>()) {
                    return std::countl_zero(value);
                } else
#endif
#if __has_builtin(__builtin_clz)
                    if constexpr (bits == int_bits) {
                    return value == 0 ? bits : __builtin_clz(value);
                } else
#endif
#if __has_builtin(__builtin_clzl)
                    if constexpr (bits == long_bits) {
                    return value == 0 ? bits : __builtin_clzl(value);
                } else
#endif
#if __has_builtin(__builtin_clzll)
                    if constexpr (bits == long_long_bits) {
                    return value == 0 ? bits : __builtin_clzll(value);
                } else
#endif
                    if constexpr ((bits == 128) && int_bits_t::contains(64)) {
                    using u64 = int_of_bits < instantiation_context<T>(64), false >;
                    u64 high = value >> 64;
                    return high == 0 ? 64 + clz<u64>(value) : clz<u64>(high);
                } else if constexpr ((bits == int_bits) || (bits == long_bits) ||
                        (bits == long_long_bits) || (bits >= 128) ||
                        ((bits > long_long_bits) && !int_bits_t::contains(128))) {
                    return bits - popcount<U>(next_pow2_sub1<U>(value));
                } else if constexpr (bits < int_bits) {
                    return clz<int>(value) + bits - int_bits;
                } else if constexpr (bits < long_bits) {
                    return clz<long>(value) + bits - long_bits;
                } else if constexpr (bits < long_long_bits) {
                    return clz<long long>(value) + bits - long_long_bits;
                } else if constexpr ((bits < 128) && int_bits_t::contains(128)) {
                    using u128 = int_of_bits < instantiation_context<T>(128), false >;
                    return clz<u128>(value) + bits - 128;
                }
            }

            template<typename T, typename U = type_traits::make_unsigned<T>,
            size_t bits = type_traits::get_bits<T>()>
            constexpr inline int ctz(T tmp) noexcept {
                using namespace type_traits;
                U value = tmp;
#if __cpp_lib_bitops >= 201907L
                if constexpr (is_native_int<U>()) {
                    return std::countr_zero(value);
                } else
#endif
#if __has_builtin(__builtin_clz)
                    if constexpr (bits == int_bits) {
                    return value == 0 ? bits : __builtin_ctz(value);
                } else
#endif
#if __has_builtin(__builtin_clzl)
                    if constexpr (bits == long_bits) {
                    return value == 0 ? bits : __builtin_ctzl(value);
                } else
#endif
#if __has_builtin(__builtin_clzll)
                    if constexpr (bits == long_long_bits) {
                    return value == 0 ? bits : __builtin_ctzll(value);
                } else
#endif
                    if constexpr ((bits == 128) && int_bits_t::contains(64)) {
                    using u64 = int_of_bits < instantiation_context<T>(64), false >;
                    u64 low = value;
                    return low == 0 ? 64 + ctz<u64>(value >> 64) : ctz<u64>(low);
                } else if constexpr ((bits == int_bits) || (bits == long_bits) ||
                        (bits == long_long_bits) || (bits >= 128) ||
                        ((bits > long_long_bits) && !int_bits_t::contains(128))) {
                    return popcount<U>(U(value & U(-value)) - U(1));
                } else if constexpr (bits < int_bits) {
                    return value == 0 ? bits : ctz<int>(value);
                } else if constexpr (bits < long_bits) {
                    return value == 0 ? bits : ctz<long>(value);
                } else if constexpr (bits < long_long_bits) {
                    return value == 0 ? bits : ctz<long long>(value);
                } else if constexpr ((bits < 128) && int_bits_t::contains(128)) {
                    using u128 = int_of_bits < instantiation_context<T>(128), false >;
                    return value == 0 ? bits : ctz<u128>(value);
                }
            }
        }

        namespace impl {

            template<typename mem_tree, bool sig>
            struct native_integer_base;

            template<typename mem_tree, bool sig>
            struct native_integer_impl;

            template<typename mem_tree>
            struct native_integer_base<mem_tree, false> {
                constexpr static size_t size = seq::tree_get<mem_tree>();
                using S = type_traits::int_of_size<size, true>;
                using U = type_traits::int_of_size<size, false>;
                using I = native_integer_base;
                using M = utils::sh_type<size>;
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
                    //todo
                    bool tmp = add_overflow(v1, v2, out);
                    return add_overflow(out, I(cf), out) | tmp;
                }

                constexpr static bool
                sub_carry(I v1, I v2, bool cf, I &out) noexcept {
                    //todo
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

            template<typename mem_tree>
            struct native_integer_base<mem_tree, true> {
                constexpr static size_t size = seq::tree_get<mem_tree>();
                using S = type_traits::int_of_size<size, true>;
                using U = type_traits::int_of_size<size, false>;
                using I = native_integer_base;
                using M = utils::sh_type<size>;
                constexpr static M max_sh = size * type_traits::min_native_bits - 1;
                U value;

                constexpr native_integer_base() noexcept = default;

                constexpr explicit native_integer_base(U n) noexcept : value(n) { }

                constexpr bool is_negative() const noexcept {
                    return S(value) < 0;
                }

                constexpr I operator/(I other) const noexcept {
                    //todo
                    return I(S(value) / S(other.value));
                }

                constexpr I operator%(I other) const noexcept {
                    //todo
                    return I(S(value) % S(other.value));
                }

                constexpr I operator>>(M other) const noexcept {
                    return I(S(value) >> (other > max_sh ? max_sh : other));
                }

                constexpr bool operator>(I other) const noexcept {
                    return S(value) > S(other.value);
                }

                constexpr bool operator<(I other) const noexcept {
                    return S(value) < S(other.value);
                }

                constexpr bool operator>=(I other) const noexcept {
                    return S(value) >= S(other.value);
                }

                constexpr bool operator<=(I other) const noexcept {
                    return S(value) <= S(other.value);
                }
            };

            template<typename mem_tree, bool sig>
            struct native_integer_impl : public native_integer_base<mem_tree, sig> {
                static_assert(mem_tree::length == 0, "native mem_tree must be empty");
                constexpr static size_t size = seq::tree_get<mem_tree>();
                using I = native_integer_impl;
                using UI = native_integer_impl<mem_tree, false>;
                using SI = native_integer_impl<mem_tree, true>;
                using B = native_integer_base<mem_tree, sig>;
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
                    return I(other > B::max_sh ? 0 : value << other);
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

    template<typename mem_tree, bool sig>
    class tree_integer;

    template<i_detail::size_t size, bool sig, typename mem_seq>
    class seq_integer;
}

#ifndef INTEGER_HPP_HAS_BUILTIN_CHECK
#undef __has_builtin
#else
#undef INTEGER_HPP_HAS_BUILTIN_CHECK
#endif

#undef INTEGER_HPP_HAS_OSTREAM
#undef INTEGER_HPP_HAS_BIT
#undef INTEGER_HPP_HAS_LIMITS

#endif /* INTEGER_HPP */

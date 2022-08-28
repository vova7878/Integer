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

#include <cstdint>
#include <type_traits>
#include <cstddef>
#include <iostream>

// -std=c++14
namespace JIO {

    template<bool A>
    using p_enable_if_t = typename std::enable_if<A, bool>::type;

#define p_enable_if(B) p_enable_if_t<(B)> = false

    template<typename T, bool v, p_enable_if(v)>
    using p_cond_t = T;

    template<typename T>
    constexpr inline T lowestOneBit(const T i) noexcept {
        // HD, Section 2-1
        return i & -i;
    }

    template<typename T>
    constexpr inline bool isOneBit(const T i) noexcept {
        return lowestOneBit(i) == i;
    }

    template<typename T>
    constexpr const T& max(const T &v1, const T &v2) noexcept {
        return v1 < v2 ? v2 : v1;
    }

    template<typename T>
    constexpr const T& min(const T &v1, const T &v2) noexcept {
        return v1 < v2 ? v1 : v2;
    }

    namespace p_i_seq {

        template <typename T, T... values>
        struct array_t {

            constexpr inline const T operator[](size_t index) const noexcept {
                static_assert(sizeof...(values) != 0, "zero-size array");
                constexpr T data[sizeof...(values)] = {values...};
                return data[index];
            }
        };

        template <typename T, size_t length>
        struct v_array_t {
            T data[max(size_t(1), length)];

            constexpr inline const T& operator[](size_t index) const noexcept {
                return data[index];
            }

            constexpr inline T& operator[](size_t index) noexcept {
                return data[index];
            }
        };

        struct any {
            constexpr inline any() noexcept = default;

            template<typename T>
            constexpr inline any(T) noexcept { }
        };

        template <size_t length>
        constexpr inline void unused_array(v_array_t<any, length>) noexcept { }

        template<typename T, T... v1, T... v2>
        constexpr inline array_t<T, v1..., v2...>
        append(array_t<T, v1...>, array_t<T, v2...>) noexcept {
            return {};
        }

        template <typename T, T f, T l,
        int = (f > l) ? -1 : (l == f ? 0 : ((l - f == 1) ? 1 : 2))>
        struct seq_h;

        template <typename T, T f, T l>
        struct seq_h <T, f, l, 0> {
            typedef array_t<T> type;
        };

        template <typename T, T f, T l>
        struct seq_h <T, f, l, 1> {
            typedef array_t<T, f> type;
        };

        template <typename T, T f, T l>
        struct seq_h <T, f, l, 2> {
            constexpr static T d = l - f;
            typedef decltype(append<T>(
                    typename seq_h<T, f, f + d / 2 > ::type(),
                    typename seq_h<T, f + d / 2, f + d> ::type()
                    )) type;
        };

        template<typename T, T f, T l>
        using make_array = typename seq_h<T, f, l>::type;

        template<size_t... i1>
        struct p_wrapper {
            template<size_t>
            using any_h = any;

            template<size_t... i2, typename T>
            constexpr inline static T
            get_value(any_h<i1>..., T value, any_h<i2>...) noexcept {
                return value;
            }
        };

        template<size_t... i1, size_t... i2, typename... Tp>
        constexpr inline auto p_element_h(array_t<size_t, i1...>,
                array_t<size_t, i2...>, Tp... arr) noexcept {
            return p_wrapper<i1...>::template get_value<i2...>(arr...);
        }

        template<size_t index, typename... Tp>
        constexpr inline auto element(Tp... arr) noexcept {
            return p_element_h(make_array<size_t, 0, index>(),
                    make_array<size_t, index + 1, sizeof...(arr)>(), arr...);
        }

        template<typename... Tp>
        constexpr inline auto last_element(Tp... arr) noexcept {
            return element<sizeof...(arr) - 1 > (arr...);
        }
    }

    namespace p_i_utils {

        constexpr static char digits[62] = {
            '0', '1', '2', '3', '4',
            '5', '6', '7', '8', '9',

            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'g', 'k', 'l', 'm',
            'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'G', 'K', 'L', 'M',
            'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
        };

        template<char c, size_t index = 0, p_enable_if(digits[index] == c)>
        constexpr inline auto indexOfDigit() noexcept {
            return index < 36 ? index : index - 26;
        }

        template<char c, size_t index = 0, p_enable_if(digits[index] != c)>
        constexpr inline auto indexOfDigit() noexcept {
            return indexOfDigit<c, index + 1 > ();
        }

        constexpr inline size_t numberOfLeadingZeros2Bit_n(uint8_t i) noexcept {
            return i ? (1 - (i >> 1)) : 2;
        }

        constexpr inline size_t numberOfLeadingZeros4Bit_n(uint8_t i) noexcept {
            return (i >= 1 << 2) ?
                    numberOfLeadingZeros2Bit_n(i >> 2) :
                    numberOfLeadingZeros2Bit_n(i) + 2;
        }

        constexpr inline size_t numberOfLeadingZeros_n(uint8_t i) noexcept {
            return (i >= 1 << 4) ?
                    numberOfLeadingZeros4Bit_n(i >> 4) :
                    numberOfLeadingZeros4Bit_n(i) + 4;
        }

        constexpr inline size_t numberOfLeadingZeros_n(uint16_t i) noexcept {
            return (i >= 1 << 8) ?
                    numberOfLeadingZeros_n(uint8_t(i >> 8)) :
                    numberOfLeadingZeros_n(uint8_t(i)) + 8;
        }

        constexpr inline size_t numberOfLeadingZeros_n(uint32_t i) noexcept {
            return (i >= 1 << 16) ?
                    numberOfLeadingZeros_n(uint16_t(i >> 16)) :
                    numberOfLeadingZeros_n(uint16_t(i)) + 16;
        }

        constexpr inline size_t numberOfLeadingZeros_n(uint64_t i) noexcept {
            return (i >= uint64_t(1) << 32) ?
                    numberOfLeadingZeros_n(uint32_t(i >> 32)) :
                    numberOfLeadingZeros_n(uint32_t(i)) + 32;
        }

        constexpr inline size_t numberOfTrailingZeros2Bit_n(uint8_t i) noexcept {
            return i ? (1 - (i & 1)) : 2;
        }

        constexpr inline size_t numberOfTrailingZeros4Bit_n(uint8_t i) noexcept {
            return (i & 0x3) ?
                    numberOfTrailingZeros2Bit_n(i) :
                    numberOfTrailingZeros2Bit_n(i >> 2) + 2;
        }

        constexpr inline size_t numberOfTrailingZeros_n(uint8_t i) noexcept {
            return (i & 0xf) ?
                    numberOfTrailingZeros4Bit_n(i) :
                    numberOfTrailingZeros4Bit_n(i >> 4) + 4;
        }

        constexpr inline size_t numberOfTrailingZeros_n(uint16_t i) noexcept {
            return (uint8_t(i)) ?
                    numberOfTrailingZeros_n(uint8_t(i)) :
                    numberOfTrailingZeros_n(uint8_t(i >> 8)) + 8;
        }

        constexpr inline size_t numberOfTrailingZeros_n(uint32_t i) noexcept {
            return (uint16_t(i)) ?
                    numberOfTrailingZeros_n(uint16_t(i)) :
                    numberOfTrailingZeros_n(uint16_t(i >> 16)) + 16;
        }

        constexpr inline size_t numberOfTrailingZeros_n(uint64_t i) noexcept {
            return (uint32_t(i)) ?
                    numberOfTrailingZeros_n(uint32_t(i)) :
                    numberOfTrailingZeros_n(uint32_t(i >> 32)) + 32;
        }

        template <typename T>
        constexpr inline size_t logb2_n(const T value) noexcept {
            return sizeof (T) * 8 - numberOfLeadingZeros_n(value);
        }

        constexpr inline size_t make_pow2(size_t i) noexcept {
            if (i == 0) {
                return 0;
            }
            size_t log = logb2_n(i);
            if (i == (size_t(1) << (log - 1))) {
                return i;
            }
            return 1 << log;
        }

        template<size_t size, bool sig>
        struct native_int_type_h;

        template<>
        struct native_int_type_h<1, false> {
            typedef uint8_t type;
        };

        template<>
        struct native_int_type_h<1, true> {
            typedef int8_t type;
        };

        template<>
        struct native_int_type_h<2, false> {
            typedef uint16_t type;
        };

        template<>
        struct native_int_type_h<2, true> {
            typedef int16_t type;
        };

        template<>
        struct native_int_type_h<4, false> {
            typedef uint32_t type;
        };

        template<>
        struct native_int_type_h<4, true> {
            typedef int32_t type;
        };

        template<>
        struct native_int_type_h<8, false> {
            typedef uint64_t type;
        };

        template<>
        struct native_int_type_h<8, true> {
            typedef int64_t type;
        };

        template<size_t size, bool sig>
        using native_int_type = typename native_int_type_h<size, sig>::type;
    }

    enum p_IType {
        illegal = 0,
        native,
        pow2,
        array
    };

    constexpr p_IType p_getIntegerType(size_t size) noexcept {
        if (!size) {
            return illegal;
        }
        if (size == 1 || size == 2 || size == 4 || size == 8) {
            return native;
        }
        return isOneBit(size) ? pow2 : array;
    }

    template<size_t size, bool = (size > sizeof (unsigned int))>
    struct p_SHValue {
        typedef unsigned int type;
    };

    template<size_t size>
    struct p_SHValue<size, true> {
        typedef size_t type;
    };

    template<size_t size>
    using p_SHType = typename p_SHValue<size>::type;

    template<size_t size, bool sig>
    class Integer;

    template<size_t size, bool sig, p_IType = p_getIntegerType(size)>
    struct p_Integer_Impl;

    template<size_t size, bool sig>
    struct p_native_Integer_Impl;

    template<size_t size, bool sig>
    struct p_Integer_Impl <size, sig, native> {
        typedef p_native_Integer_Impl<size, sig> type;
    };

    template<size_t size, bool sig>
    struct p_native_Integer_Base;

    template<size_t size>
    struct p_native_Integer_Base<size, false> {
    private:
        typedef p_i_utils::native_int_type<size, true> S;
        typedef p_i_utils::native_int_type<size, false> U;
        typedef p_native_Integer_Base I;
        typedef p_SHType<sizeof (U) > M;
        constexpr static M shmask = sizeof (U) * 8 - 1;
        U value;
    public:

        constexpr inline p_native_Integer_Base() noexcept = default;

        constexpr explicit inline
        p_native_Integer_Base(const U n) noexcept : value(n) { }

        constexpr inline bool isNegative() const noexcept {
            return false;
        }

        constexpr inline static bool
        add_overflow(const I &v1, const I &v2, I &out) noexcept {
            U tmp = v1.value + v2.value;
            out = I(tmp);
            return tmp < v1.value;
        }

        constexpr inline static bool increment_overflow(I &value) noexcept {
            return (++value.value) == 0;
        }

        constexpr inline static bool
        sub_overflow(const I &v1, const I &v2, I &out) noexcept {
            U tmp = v1.value - v2.value;
            out = I(tmp);
            return tmp > v1.value;
        }

        constexpr inline static bool decrement_overflow(I &value) noexcept {
            return (value.value--) == 0;
        }

        constexpr inline I operator/(const I &other) const noexcept {
            return I(value / other.value);
        }

        constexpr inline I operator%(const I &other) const noexcept {
            return I(value % other.value);
        }

        constexpr inline I operator>>(const M other) const noexcept {
            return I(value >> (other & shmask));
        }

        constexpr inline bool operator>(const I &other) const noexcept {
            return value > other.value;
        }

        constexpr inline bool operator<(const I &other) const noexcept {
            return value < other.value;
        }

        constexpr inline bool operator>=(const I &other) const noexcept {
            return value >= other.value;
        }

        constexpr inline bool operator<=(const I &other) const noexcept {
            return value <= other.value;
        }

        template<size_t size2, bool sig2>
        friend struct p_native_Integer_Impl;

        template<size_t size2, bool sig2>
        friend class Integer;
    };

    template<size_t size>
    struct p_native_Integer_Base<size, true> {
    private:
        typedef p_i_utils::native_int_type<size, true> S;
        typedef p_i_utils::native_int_type<size, false> U;
        typedef p_native_Integer_Base I;
        typedef p_SHType<sizeof (U) > M;
        constexpr static M shmask = sizeof (U) * 8 - 1;
        U value;
    public:

        constexpr inline p_native_Integer_Base() noexcept = default;

        constexpr explicit inline
        p_native_Integer_Base(const S n) noexcept : value(n) { }

        constexpr inline bool isNegative() const noexcept {
            return S(value) < 0;
        }

        template<size_t size2, size_t = size2>
        struct divrem_h {

            constexpr inline static I div(const I &a, const I &b) noexcept {
                return I(S(a.value) / S(b.value));
            }

            constexpr inline static I rem(const I &a, const I &b) noexcept {
                return I(S(a.value) % S(b.value));
            }
        };

        template<size_t size2>
        struct divrem_h<size2, 1> {

            constexpr inline static I div(const I &a, const I &b) noexcept {
                if ((a.value == U(0x80)) && (b.value == U(-1))) {
                    return I(0x80);
                }
                return divrem_h<size2, 0>::div(a, b);
            }

            constexpr inline static I rem(const I &a, const I &b) noexcept {
                if ((a.value == U(0x80)) && (b.value == U(-1))) {
                    return I(0);
                }
                return divrem_h<size2, 0>::rem(a, b);
            }
        };

        template<size_t size2>
        struct divrem_h<size2, 2> {

            constexpr inline static I div(const I &a, const I &b) noexcept {
                if ((a.value == U(0x8000)) && (b.value == U(-1))) {
                    return I(0x8000);
                }
                return divrem_h<size2, 0>::div(a, b);
            }

            constexpr inline static I rem(const I &a, const I &b) noexcept {
                if ((a.value == U(0x8000)) && (b.value == U(-1))) {
                    return I(0);
                }
                return divrem_h<size2, 0>::rem(a, b);
            }
        };

        template<size_t size2>
        struct divrem_h<size2, 4> {

            constexpr inline static I div(const I &a, const I &b) noexcept {
                if ((a.value == U(0x80000000L)) && (b.value == U(-1L))) {
                    return I(0x80000000L);
                }
                return divrem_h<size2, 0>::div(a, b);
            }

            constexpr inline static I rem(const I &a, const I &b) noexcept {
                if ((a.value == U(0x80000000L)) && (b.value == U(-1L))) {
                    return I(0);
                }
                return divrem_h<size2, 0>::rem(a, b);
            }
        };

        template<size_t size2>
        struct divrem_h<size2, 8> {

            constexpr inline static I div(const I &a, const I &b) noexcept {
                if ((a.value == U(0x8000000000000000LL)) && (b.value == U(-1LL))) {
                    return I(0x8000000000000000LL);
                }
                return divrem_h<size2, 0>::div(a, b);
            }

            constexpr inline static I rem(const I &a, const I &b) noexcept {
                if ((a.value == U(0x8000000000000000LL)) && (b.value == U(-1LL))) {
                    return I(0);
                }
                return divrem_h<size2, 0>::rem(a, b);
            }
        };

        constexpr inline I operator/(const I &other) const noexcept {
            return divrem_h<size>::div(*this, other);
        }

        constexpr inline I operator%(const I &other) const noexcept {
            return divrem_h<size>::rem(*this, other);
        }

        constexpr inline I operator>>(const M other) const noexcept {
            return I(S(value) >> (other & shmask));
        }

        constexpr inline bool operator>(const I &other) const noexcept {
            return S(value) > S(other.value);
        }

        constexpr inline bool operator<(const I &other) const noexcept {
            return S(value) < S(other.value);
        }

        constexpr inline bool operator>=(const I &other) const noexcept {
            return S(value) >= S(other.value);
        }

        constexpr inline bool operator<=(const I &other) const noexcept {
            return S(value) <= S(other.value);
        }

        template<size_t size2, bool sig2>
        friend struct p_native_Integer_Impl;

        template<size_t size2, bool sig2>
        friend class Integer;
    };

    template<size_t size, bool sig>
    struct p_native_Integer_Impl : public p_native_Integer_Base<size, sig> {
    private:
        typedef p_native_Integer_Base<size, sig> T;
        typedef p_native_Integer_Impl<size, sig> I;
        typedef p_native_Integer_Impl<size, false> UI;
        typedef p_native_Integer_Impl<size, true> SI;
        typedef typename T::U U;
        typedef typename T::S S;
        typedef typename T::M M;

    public:

        using T::T;

        constexpr inline p_native_Integer_Impl() noexcept = default;

        constexpr inline p_native_Integer_Impl(const T &obj) noexcept : T(obj) { }

        constexpr inline static I ZERO() noexcept {
            return T(0);
        }

        constexpr inline bool isZero() const noexcept {
            return T::value == 0;
        }

        constexpr inline bool upperBit() const noexcept {
            return S(T::value) < 0;
        }

        constexpr inline I addOne() const noexcept {
            return I(T::value + 1);
        }

        constexpr inline I subOne() const noexcept {
            return I(T::value - 1);
        }

        template<size_t index, p_enable_if(index < size)>
        constexpr inline uint8_t getByte() const noexcept {
            return T::value >> (index * 8);
        }

        template<size_t index, U mask = U(~(U(0xffU) << (index * 8))),
        p_enable_if(index < size)>
        constexpr inline void setByte(uint8_t v) noexcept {
            T::value = (T::value & mask) | (U(v) << index * 8);
        }

        template<size_t index, U mask = U(U(1) << index),
        p_enable_if(index < size * 8)>
        constexpr inline bool getBit() const noexcept {
            return T::value & mask;
        }

        template<size_t index, U mask1 = U(U(1) << index),
        U mask2 = U(~mask1), p_enable_if(index < size * 8)>
        constexpr inline void setBit(bool v) noexcept {
            T::value = v ? (T::value | mask1) : (T::value & mask2);
        }

        constexpr inline static bool
        add_overflow(const I &v1, const I &v2, I &out) noexcept {
            return T::add_overflow(v1, v2, out);
        }

        constexpr inline static bool increment_overflow(I &value) noexcept {
            return T::increment_overflow(value);
        }

        constexpr inline static bool
        sub_overflow(const I &v1, const I &v2, I &out) noexcept {
            return T::sub_overflow(v1, v2, out);
        }

        constexpr inline static bool decrement_overflow(I &value) noexcept {
            return T::decrement_overflow(value);
        }

        constexpr inline static void
        leftShiftOneBit(I &value, bool bit) noexcept {
            value.value = (value.value << 1) | U(bit);
        }

        constexpr inline size_t numberOfLeadingZeros() const noexcept {
            return p_i_utils::numberOfLeadingZeros_n(T::value);
        }

        constexpr inline size_t numberOfTrailingZeros() const noexcept {
            return p_i_utils::numberOfTrailingZeros_n(T::value);
        }

        constexpr inline I operator+() const noexcept {
            return *this;
        }

        constexpr inline I operator-() const noexcept {
            return I(-T::value);
        }

        constexpr inline I operator+(const I &other) const noexcept {
            return I(T::value + other.value);
        }

        constexpr inline I operator-(const I &other) const noexcept {
            return I(T::value - other.value);
        }

        constexpr inline I operator*(const I &other) const noexcept {
            return I(T::value * other.value);
        }

        constexpr inline I operator|(const I &other) const noexcept {
            return I(T::value | other.value);
        }

        constexpr inline I operator&(const I &other) const noexcept {
            return I(T::value & other.value);
        }

        constexpr inline I operator^(const I &other) const noexcept {
            return I(T::value ^ other.value);
        }

        constexpr inline I operator<<(const M other) const noexcept {
            return I(T::value << (other & T::shmask));
        }

        constexpr inline bool operator==(const I &other) const noexcept {
            return T::value == other.value;
        }

        constexpr inline bool operator!=(const I &other) const noexcept {
            return T::value != other.value;
        }

        constexpr inline I operator~() const noexcept {
            return I(~T::value);
        }

        template<size_t size2, bool sig2>
        friend class Integer;
    };

    template<size_t half, bool sig>
    class p_pow2_Integer_Impl;

    template<size_t size, bool sig>
    struct p_Integer_Impl <size, sig, pow2> {
        typedef p_pow2_Integer_Impl<size / 2, sig> type;
    };

    template<size_t half, bool sig>
    class p_pow2_Integer_Base;

    template<size_t half>
    class p_pow2_Integer_Base<half, false> {
    private:
        typedef Integer<half, true> S;
        typedef Integer<half, false> U;
        typedef p_pow2_Integer_Base<half, false> I;
        typedef p_SHType<half * 2> M;
        constexpr static M shmask = half * 2 * 8 - 1;
        U low, high;

        constexpr inline static I
        rightShift(const I &value, const M shiftDistance) noexcept {
            if (shiftDistance == 0) {
                return value;
            }
            if (shiftDistance < (half * 8)) {
                return I((value.low >> shiftDistance) |
                        (value.high << (half * 8 - shiftDistance)),
                        value.high >> shiftDistance);
            }
            return I(value.high >> (shiftDistance - half * 8), U::ZERO());
        }
    public:

        constexpr inline p_pow2_Integer_Base() noexcept = default;

        constexpr explicit inline p_pow2_Integer_Base(const U &low) noexcept :
        low(low), high(U::ZERO()) { }

        constexpr explicit inline p_pow2_Integer_Base(const S &low) noexcept :
        low(low), high(low.upperBit() ? ~U::ZERO() : U::ZERO()) { }

        constexpr inline p_pow2_Integer_Base(const U &low, const U &high)
        noexcept : low(low), high(high) { }

        constexpr inline bool isNegative() const noexcept {
            return false;
        }

        constexpr inline static bool
        add_overflow(const I &v1, const I &v2, I &out) noexcept {
            bool tmpo = U::add_overflow(v1.high, v2.high, out.high);
            if (U::add_overflow(v1.low, v2.low, out.low)) {
                return U::increment_overflow(out.high) || tmpo;
            }
            return tmpo;
        }

        constexpr inline static bool increment_overflow(I &value) noexcept {
            if (U::increment_overflow(value.low)) {
                return U::increment_overflow(value.high);
            }
            return false;
        }

        constexpr inline static bool
        sub_overflow(const I &v1, const I &v2, I &out) noexcept {
            bool tmpo = U::sub_overflow(v1.high, v2.high, out.high);
            if (U::sub_overflow(v1.low, v2.low, out.low)) {
                return U::decrement_overflow(out.high) || tmpo;
            }
            return tmpo;
        }

        constexpr inline static bool decrement_overflow(I &value) noexcept {
            if (U::decrement_overflow(value.low)) {
                return U::decrement_overflow(value.high);
            }
            return false;
        }

        constexpr inline bool operator>(const I &other) const noexcept {
            if (high == other.high) {
                return low > other.low;
            }
            return high > other.high;
        }

        constexpr inline bool operator<(const I &other) const noexcept {
            if (high == other.high) {
                return low < other.low;
            }
            return high < other.high;
        }

        constexpr inline bool operator>=(const I &other) const noexcept {
            if (high == other.high) {
                return low >= other.low;
            }
            return high > other.high;
        }

        constexpr inline bool operator<=(const I &other) const noexcept {
            if (high == other.high) {
                return low <= other.low;
            }
            return high < other.high;
        }

        constexpr inline I operator>>(const M other) const noexcept {
            return rightShift(*this, other & shmask);
        }

        template<size_t size2, bool sig2>
        friend class p_pow2_Integer_Impl;

        template<size_t size2, bool sig2>
        friend class Integer;
    };

    template<size_t half>
    class p_pow2_Integer_Base<half, true> {
    private:
        typedef Integer<half, true> S;
        typedef Integer<half, false> U;
        typedef p_pow2_Integer_Base<half, true> I;
        typedef p_SHType<half * 2> M;
        constexpr static M shmask = half * 2 * 8 - 1;
        U low, high;

        constexpr inline static I
        rightShift(const I &value, const M shiftDistance) noexcept {
            if (shiftDistance == 0) {
                return value;
            }
            if (shiftDistance < (half * 8)) {
                return I((value.low >> shiftDistance) |
                        (value.high << (half * 8 - shiftDistance)),
                        S(value.high) >> shiftDistance);
            }
            return I(S(value.high) >> (shiftDistance - half * 8),
                    value.high.upperBit() ? ~U::ZERO() : U::ZERO());
        }
    public:

        constexpr inline p_pow2_Integer_Base() noexcept = default;

        constexpr explicit inline p_pow2_Integer_Base(const U &low) noexcept :
        low(low), high(U::ZERO()) { }

        constexpr explicit inline p_pow2_Integer_Base(const S &low) noexcept :
        low(low), high(low.isNegative() ? ~U::ZERO() : U::ZERO()) { }

        constexpr inline p_pow2_Integer_Base(const U &low, const U &high)
        noexcept : low(low), high(high) { }

        constexpr inline bool isNegative() const noexcept {
            return high.upperBit();
        }

        constexpr inline bool operator>(const I &other) const noexcept {
            if (high == other.high) {
                return low > other.low;
            }
            return S(high) > S(other.high);
        }

        constexpr inline bool operator<(const I &other) const noexcept {
            if (high == other.high) {
                return low < other.low;
            }
            return S(high) < S(other.high);
        }

        constexpr inline bool operator>=(const I &other) const noexcept {
            if (high == other.high) {
                return low >= other.low;
            }
            return S(high) > S(other.high);
        }

        constexpr inline bool operator<=(const I &other) const noexcept {
            if (high == other.high) {
                return low <= other.low;
            }
            return S(high) < S(other.high);
        }

        constexpr inline I operator>>(const M other) const noexcept {
            return rightShift(*this, other & shmask);
        }

        template<size_t size2, bool sig2>
        friend class p_pow2_Integer_Impl;

        template<size_t size2, bool sig2>
        friend class Integer;
    };

    template<size_t half, bool sig>
    class p_pow2_Integer_Impl : public p_pow2_Integer_Base<half, sig> {
    private:
        typedef p_pow2_Integer_Base<half, sig> T;
        typedef p_pow2_Integer_Impl<half, sig> I;
        typedef p_pow2_Integer_Impl<half, false> UI;
        typedef p_pow2_Integer_Impl<half, true> SI;
        typedef typename T::S S;
        typedef typename T::U U;
        typedef typename T::M M;

        constexpr inline static I
        leftShift(const I &value, const typename T::M shiftDistance) noexcept {
            if (shiftDistance == 0) {
                return value;
            }
            if (shiftDistance < (half * 8)) {
                return I(value.low << shiftDistance,
                        (value.high << shiftDistance) |
                        (value.low >> (half * 8 - shiftDistance)));
            }
            return I(U::ZERO(), value.low << (shiftDistance - half * 8));
        }

    public:

        using T::T;

        constexpr inline p_pow2_Integer_Impl() noexcept = default;

        template<bool sig2>
        constexpr inline
        p_pow2_Integer_Impl(const p_pow2_Integer_Impl<half, sig2> &other)
        noexcept : T(other.low, other.high) { }

        constexpr inline p_pow2_Integer_Impl(const T &obj) noexcept : T(obj) { }

        constexpr inline SI s() const noexcept {
            return SI(T::low, T::high);
        }

        constexpr inline UI u() const noexcept {
            return UI(T::low, T::high);
        }

        constexpr inline static I ZERO() noexcept {
            return I(U::ZERO());
        }

        constexpr inline bool isZero() const noexcept {
            return T::low.isZero() && T::high.isZero();
        }

        constexpr inline bool upperBit() const noexcept {
            return T::high.upperBit();
        }

        constexpr inline I addOne() const noexcept {
            I tmp = *this;
            if (U::increment_overflow(tmp.low)) {
                ++tmp.high;
            }
            return tmp;
        }

        constexpr inline I subOne() const noexcept {
            I tmp = *this;
            if (U::decrement_overflow(tmp.low)) {
                --tmp.high;
            }
            return tmp;
        }

        template<size_t index, p_enable_if(index < half)>
        constexpr inline uint8_t getByte() const noexcept {
            return T::low.template getByte<index>();
        }

        template<size_t index, p_enable_if((index >= half) && (index < half * 2))>
        constexpr inline uint8_t getByte() const noexcept {
            return T::high.template getByte < index - half > ();
        }

        template<size_t index, p_enable_if(index < half)>
        constexpr inline void setByte(uint8_t v) noexcept {
            T::low.template setByte<index>(v);
        }

        template<size_t index, p_enable_if((index >= half) && (index < half * 2))>
        constexpr inline void setByte(uint8_t v) noexcept {
            T::high.template setByte < index - half > (v);
        }

        template<size_t index, p_enable_if(index < half * 8)>
        constexpr inline bool getBit() const noexcept {
            return T::low.template getBit<index>();
        }

        template<size_t index, p_enable_if((index >= half * 8) && (index < half * 16))>
        constexpr inline bool getBit() const noexcept {
            return T::high.template getBit < index - half * 8 > ();
        }

        template<size_t index, p_enable_if(index < half * 8)>
        constexpr inline void setBit(bool v) noexcept {
            T::low.template setBit<index>(v);
        }

        template<size_t index, p_enable_if((index >= half * 8) && (index < half * 16))>
        constexpr inline void setBit(bool v) noexcept {
            T::high.template setBit < index - half * 8 > (v);
        }

        constexpr inline static bool add_overflow(
                const I &v1, const I &v2, I &out) noexcept {
            return T::add_overflow(v1, v2, out);
        }

        constexpr inline static bool increment_overflow(I &value) noexcept {
            return T::increment_overflow(value);
        }

        constexpr inline static bool sub_overflow(
                const I &v1, const I &v2, I &out) noexcept {
            return T::sub_overflow(v1, v2, out);
        }

        constexpr inline static bool decrement_overflow(I &value) noexcept {
            return T::decrement_overflow(value);
        }

        constexpr inline static void
        leftShiftOneBit(I &value, bool bit) noexcept {
            U::leftShiftOneBit(value.high, value.low.upperBit());
            U::leftShiftOneBit(value.low, bit);
        }

        constexpr inline size_t numberOfLeadingZeros() const noexcept {
            if (T::high.isZero()) {
                return T::low.numberOfLeadingZeros() + half * 8;
            }
            return T::high.numberOfLeadingZeros();
        }

        constexpr inline size_t numberOfTrailingZeros() const noexcept {
            if (T::low.isZero()) {
                return T::high.numberOfTrailingZeros() + half * 8;
            }
            return T::low.numberOfTrailingZeros();
        }

        constexpr inline I operator+() const noexcept {
            return *this;
        }

        constexpr inline I operator-() const noexcept {
            return (~(*this)).addOne();
        }

        template<typename II = Integer<half * 2, sig>>
        constexpr inline I operator/(const I &other) const noexcept {
            return divrem(II(*this), II(other)).value;
        }

        template<typename II = Integer<half * 2, sig>>
        constexpr inline I operator%(const I &other) const noexcept {
            II rem;
            divrem(II(*this), II(other), &rem);
            return rem.value;
        }

        constexpr inline I operator+(const I &other) const noexcept {
            U tmph = T::high + other.high;
            U tmpl = U::ZERO();
            if (U::add_overflow(T::low, other.low, tmpl)) {
                ++tmph;
            }
            return I(tmpl, tmph);
        }

        constexpr inline I operator-(const I &other) const noexcept {
            U tmph = T::high - other.high;
            U tmpl = U::ZERO();
            if (U::sub_overflow(T::low, other.low, tmpl)) {
                --tmph;
            }
            return I(tmpl, tmph);
        }

        constexpr inline I operator*(const I &other) const noexcept {
            auto tmp = wmultiply(T::low, other.low).value;
            return I(tmp.low, tmp.high + T::low * other.high + T::high * other.low);
        }

        constexpr inline I operator<<(const M other) const noexcept {
            return leftShift(*this, other & T::shmask);
        }

        constexpr inline I operator|(const I &other) const noexcept {
            return I(T::low | other.low, T::high | other.high);
        }

        constexpr inline I operator&(const I &other) const noexcept {
            return I(T::low & other.low, T::high & other.high);
        }

        constexpr inline I operator^(const I &other) const noexcept {
            return I(T::low ^ other.low, T::high ^ other.high);
        }

        constexpr inline I operator~() const noexcept {
            return I(~T::low, ~T::high);
        }

        constexpr inline bool operator==(const I &other) const noexcept {
            return (T::low == other.low) && (T::high == other.high);
        }

        constexpr inline bool operator!=(const I &other) const noexcept {
            return (T::low != other.low) || (T::high != other.high);
        }

        template<size_t size2, bool sig2>
        friend class Integer;
    };

    template<typename T1, typename T2>
    struct p_compare_types {
        constexpr static bool value = false;
    };

    template<typename T>
    struct p_compare_types <T, T> {
        constexpr static bool value = true;
    };

    template<typename T1, typename T2>
    constexpr inline bool p_compare_types_cv() noexcept {
        return p_compare_types<typename std::remove_cv<T1>::type,
                typename std::remove_cv<T2>::type>::value;
    }

    template<typename T>
    constexpr inline bool p_is_integral_no_bool() noexcept {
        return std::is_integral<T>::value && (!p_compare_types_cv<T, bool>());
    }

    template<typename T>
    using p_int_t = p_cond_t<T, p_is_integral_no_bool<T>()>;

    template<typename T>
    using p_bool_t = p_cond_t<T, p_compare_types_cv<T, bool>()>;

    template<typename T>
    constexpr inline bool p_is_signed() noexcept {
        return std::is_signed<T>::value;
    }

    template<typename U, typename S, bool sig, typename V, p_enable_if(sig)>
    constexpr inline S p_castUS(V value) noexcept {
        return S(value);
    }

    template<typename U, typename S, bool sig, typename V, p_enable_if(!sig)>
    constexpr inline U p_castUS(V value) noexcept {
        return U(value);
    }

    template<typename T, bool sig, p_enable_if(sig)>
    constexpr inline T p_min_value() noexcept {
        return T(1) << (sizeof (T) * 8 - 1);
    }

    template<typename T, bool sig, p_enable_if(!sig)>
    constexpr inline T p_min_value() noexcept {
        return T::ZERO();
    }

    template<typename T, bool sig>
    constexpr inline T p_max_value() noexcept {
        return ~p_min_value<T, sig>();
    }

    template<size_t size, typename UI = Integer<size, false>,
    p_enable_if(p_getIntegerType(size) == pow2)>
    constexpr inline UI p_udivrem(const Integer<size, false> &a,
            const Integer<size, false> &b, Integer<size, false> *rem) noexcept {
        constexpr size_t n_word_bits = size * 8;
        constexpr size_t n_half_bits = size * 4;
        UI q = UI::ZERO();
        UI r = UI::ZERO();
        size_t sr = 0;
        // special cases, X is unknown, K != 0
        if (a.uhigh() == 0) {
            if (b.uhigh() == 0) {
                // 0 X
                // ---
                // 0 X
                if (rem) {
                    rem->uhigh() = 0;
                    return divrem(a.ulow(), b.ulow(), &(rem->ulow()));
                }
                return a.ulow() / b.ulow();
            }
            // 0 X
            // ---
            // K X
            if (rem)
                *rem = a.ulow();
            return 0;
        }
        // a.uhigh() != 0
        if (b.ulow() == 0) {
            if (b.uhigh() == 0) {
                // K X
                // ---
                // 0 0
                if (rem) {
                    *rem = a.uhigh() % b.ulow();
                }
                return a.uhigh() / b.ulow();
            }
            // b.uhigh() != 0
            if (a.ulow() == 0) {
                // K 0
                // ---
                // K 0
                if (rem) {
                    rem->ulow() = 0;
                    return divrem(a.uhigh(), b.uhigh(), &(rem->uhigh()));
                }
                return a.uhigh() / b.uhigh();
            }
            // K K
            // ---
            // K 0
            if (isOneBit(b.uhigh())) /* if b is a power of 2 */ {
                if (rem) {
                    r.ulow() = a.ulow();
                    r.uhigh() = a.uhigh() & (b.uhigh() - 1);
                    *rem = r;
                }
                return a.uhigh() >> b.uhigh().numberOfTrailingZeros();
            }
            // K K
            // ---
            // K 0
            sr = b.uhigh().numberOfLeadingZeros() - a.uhigh().numberOfLeadingZeros();
            // 0 <= sr <= n_half_bits - 2 or sr large
            if (sr > n_half_bits - 2) {
                if (rem) {
                    *rem = a;
                }
                return 0;
            }
            ++sr;
            // 1 <= sr <= n_half_bits - 1
            // q = a << (n_word_bits - sr);
            q.ulow() = 0;
            q.uhigh() = a.ulow() << (n_half_bits - sr);

            r = a >> sr;
        } else /* b.ulow() != 0 */ {
            if (b.uhigh() == 0) {
                // K X
                // ---
                // 0 K
                if (isOneBit(b.ulow())) /* if b is a power of 2 */ {
                    if (rem) {
                        *rem = a.ulow() & (b.ulow() - 1);
                    }
                    if (b.ulow() == 1) {
                        return a;
                    }
                    sr = b.ulow().numberOfTrailingZeros();
                    return a >> sr;
                }
                // K X
                // ---
                // 0 K
                sr = 1 + n_half_bits + b.ulow().numberOfLeadingZeros() -
                        a.uhigh().numberOfLeadingZeros();
                // 2 <= sr <= n_word_bits - 1
                // q = a << (n_word_bits - sr);
                // r = a >> sr;
                if (sr == n_half_bits) {
                    q.ulow() = 0;
                    q.uhigh() = a.ulow();
                    r.uhigh() = 0;
                    r.ulow() = a.uhigh();
                } else if (sr < n_half_bits) /* 2 <= sr <= n_half_bits - 1 */ {
                    q.ulow() = 0;
                    q.uhigh() = a.ulow() << (n_half_bits - sr);
                    r = a >> sr;
                } else /* n_half_bits + 1 <= sr <= n_word_bits - 1 */ {
                    q = a << (n_word_bits - sr);
                    r.uhigh() = 0;
                    r.ulow() = a.uhigh() >> (sr - n_half_bits);
                }
            } else {
                // K X
                // ---
                // K K
                sr = b.uhigh().numberOfLeadingZeros() - a.uhigh().numberOfLeadingZeros();
                // 0 <= sr <= n_half_bits - 1 or sr large
                if (sr > n_half_bits - 1) {
                    if (rem)
                        *rem = a;
                    return 0;
                }
                ++sr;
                // 1 <= sr <= n_half_bits
                // q = a << (n_word_bits - sr);
                // r = a >> sr;
                q.ulow() = 0;
                if (sr == n_half_bits) {
                    q.uhigh() = a.ulow();
                    r.uhigh() = 0;
                    r.ulow() = a.uhigh();
                } else {
                    r = a >> sr;
                    q.uhigh() = a.ulow() << (n_half_bits - sr);
                }
            }
        }
        // Not a special case
        // q and r are initialized with:
        // q = a << (n_word_bits - sr);
        // r = a >> sr;
        // 1 <= sr <= n_word_bits - 1
        bool carry = 0;
        for (; sr > 0; --sr) {
            // r:q = ((r:q)  << 1) | carry
            UI::leftShiftOneBit(r, q.upperBit());
            UI::leftShiftOneBit(q, carry);

            carry = false;
            if (r >= b) {
                r -= b;
                carry = true;
            }
        }
        UI::leftShiftOneBit(q, carry);
        if (rem) {
            *rem = r;
        }
        return q;
    }

    template<size_t size, typename UI = Integer<size, false>,
    typename SI = Integer<size, true>,
    p_enable_if(p_getIntegerType(size) == pow2)>
    constexpr inline SI p_sdivrem(const Integer<size, true> &x,
            const Integer<size, true> &y, Integer<size, true> *rem) noexcept {
        bool xNeg = x.isNegative();
        bool yNeg = y.isNegative();
        UI q;
        if (rem) {
            UI r;
            q = p_udivrem<size>(xNeg ? -x : x, yNeg ? -y : y, &r);
            *rem = xNeg ? -r : r;
        } else {
            q = p_udivrem<size>(xNeg ? -x : x, yNeg ? -y : y, nullptr);
        }
        return xNeg ^ yNeg ? -q : q;
    }

    template<size_t size, bool sig, p_IType = p_getIntegerType(size)>
    struct p_divrem_h;

    template<size_t size, bool sig>
    struct p_divrem_h<size, sig, native> {

        template<typename I = Integer<size, sig>>
        constexpr inline static I divrem(
                const I &x, const I &y, I *rem) noexcept {
            if (rem) {
                *rem = x % y;
            }
            return x / y;
        }
    };

    template<size_t size>
    struct p_divrem_h<size, false, pow2> {

        template<typename UI = Integer<size, false >>
        constexpr inline static UI divrem(
                const UI &x, const UI &y, UI *rem) noexcept {
            return p_udivrem(x, y, rem);
        }
    };

    template<size_t size>
    struct p_divrem_h<size, true, pow2> {

        template<typename SI = Integer<size, true >>
        constexpr inline static SI divrem(
                const SI &x, const SI &y, SI *rem) noexcept {
            return p_sdivrem(x, y, rem);
        }
    };

    template<size_t size, bool sig>
    constexpr inline Integer<size, sig> divrem(const Integer<size, sig> &x,
            const Integer<size, sig> &y) noexcept {
        return p_divrem_h<size, sig>::divrem(x, y, (Integer<size, sig>*)nullptr);
    }

    template<size_t size, bool sig>
    constexpr inline Integer<size, sig> divrem(const Integer<size, sig> &x,
            const Integer<size, sig> &y, Integer<size, sig> *rem) noexcept {
        return p_divrem_h<size, sig>::divrem(x, y, rem);
    }

    template<size_t size, typename R = Integer<size * 2, false>,
    p_enable_if((size == 1) || (size == 2) || (size == 4))>
    constexpr inline R
    wmultiply(const Integer<size, false> &v1,
            const Integer<size, false> &v2) noexcept {
        return R(v1) * R(v2);
    }

    template<size_t size,
    p_enable_if((size == 8) || (p_getIntegerType(size) == pow2))>
    constexpr inline Integer<size * 2, false> wmultiply(
            const Integer<size, false> &v1,
            const Integer<size, false> &v2) noexcept {
        using U1 = Integer < size / 2, false >;
        using U2 = Integer<size, false>;
        using U4 = Integer<size * 2, false>;
        U1 a = U1(v1 >> (size * 4)), b = U1(v1);
        U1 c = U1(v2 >> (size * 4)), d = U1(v2);
        U2 ac = wmultiply(a, c);
        U2 bd = wmultiply(b, d);
#if 1 //The Karatsuba algorithm
        U1 ab = U1::ZERO(), cd = U1::ZERO();
        bool abo = U1::add_overflow(a, b, ab);
        bool cdo = U1::add_overflow(c, d, cd);
        U2 abcd_low = wmultiply(ab, cd);
        unsigned int abcd_high = abo && cdo;
        if (abo && U2::add_overflow(U2(abcd_low), U2(cd) << (size * 4), abcd_low)) {
            ++abcd_high;
        }
        if (cdo && U2::add_overflow(U2(abcd_low), U2(ab) << (size * 4), abcd_low)) {
            ++abcd_high;
        }
        if (U2::sub_overflow(U2(abcd_low), ac, abcd_low)) {
            --abcd_high;
        }
        if (U2::sub_overflow(U2(abcd_low), bd, abcd_low)) {
            --abcd_high;
        }
        U4 abcd = U4(abcd_low, abcd_high);
#else
        U2 ad = wmultiply(a, d);
        U2 bc = wmultiply(b, c);
        U4 abcd = U4(ad) + U4(bc);
#endif
        return U4(bd, ac) + (abcd << (size * 4));
    }

    template <size_t size, bool sig>
    constexpr inline size_t logb2(const Integer<size, sig> &value) noexcept {
        return size * 8 - value.numberOfLeadingZeros();
    }

    template<size_t size1, size_t size2, bool sig1, bool sig2>
    using result_t = Integer<max(size1, size2),
    (size1 == size2) ? sig1 && sig2 :
    (size1 > size2 ? sig1 : sig2)>;

    template<size_t size, bool sig>
    class Integer {
    private:
        using V = typename p_Integer_Impl<size, sig>::type;
        V value;

        template<size_t size2, bool sig2,
        bool = (p_getIntegerType(size) == native) &&
        (p_getIntegerType(size2) == native),
        bool = (size == size2) && (p_getIntegerType(size) == pow2),
        bool = (size != size2) &&
        ((p_getIntegerType(size) == native) ||
                (p_getIntegerType(size) == pow2)) &&
        (p_getIntegerType(size2) == pow2)>
        struct upcast_h {
        };

        template<size_t size2, bool sig2>
        struct upcast_h<size2, sig2, true, false, false> {

            constexpr inline static Integer<size2, sig2>
            upcast(const Integer &v) noexcept {
                using I = Integer<size2, sig2>;
                return typename I::V(p_castUS<typename V::U,
                        typename V::S, sig > (v.value.value));
            }
        };

        template<size_t size2, bool sig2>
        struct upcast_h<size2, sig2, false, true, false> {

            constexpr inline static Integer<size2, sig2>
            upcast(const Integer &v) noexcept {
                using I = Integer<size2, sig2>;
                return I(typename I::V(v.value.low, v.value.high));
            }
        };

        template<size_t size2, bool sig2>
        struct upcast_h<size2, sig2, false, false, true> {

            constexpr inline static Integer<size2, sig2>
            upcast(const Integer &v) noexcept {
                using I = Integer<size2, sig2>;
                return typename I::V(p_castUS<typename I::V::U,
                        typename I::V::S, sig > (v));
            }
        };

        template<size_t size2, bool sig2>
        constexpr inline Integer<size2, sig2> upcast() const noexcept {
            return upcast_h<size2, sig2>::upcast(*this);
        }

        template<size_t size2, bool sig2,
        bool = (p_getIntegerType(size) == native) &&
        (p_getIntegerType(size2) == native),
        bool = (p_getIntegerType(size) == pow2) &&
        ((p_getIntegerType(size2) == pow2) ||
                (p_getIntegerType(size2) == native))>
        struct downcast_h {
        };

        template<size_t size2, bool sig2>
        struct downcast_h<size2, sig2, true, false> {

            constexpr inline static Integer<size2, sig2>
            downcast(const Integer &v) noexcept {
                using I = Integer<size2, sig2>;
                return typename I::V(typename I::V::U(v.value.value));
            }
        };

        template<size_t size2, bool sig2>
        struct downcast_h<size2, sig2, false, true> {

            constexpr inline static Integer<size2, sig2>
            downcast(const Integer &v) noexcept {
                using I = Integer<size2, sig2>;
                return I(v.value.low);
            }
        };

        template<size_t size2, bool sig2>
        constexpr inline Integer<size2, sig2> downcast() const noexcept {
            return downcast_h<size2, sig2>::downcast(*this);
        }

        constexpr inline Integer(const V n) noexcept : value(n) { }
    public:

        constexpr inline static size_t SIZE() noexcept {
            return size;
        }

        constexpr inline static bool is_signed() noexcept {
            return sig;
        }

        constexpr inline static Integer ZERO() noexcept {
            return V::ZERO();
        }

        constexpr inline static Integer ONE() noexcept {
            return ZERO().addOne();
        }

        constexpr inline static Integer MAX_VALUE() noexcept {
            return p_max_value<Integer, sig>();
        }

        constexpr inline static Integer MIN_VALUE() noexcept {
            return p_min_value<Integer, sig>();
        }

        constexpr inline Integer() noexcept = default;

        constexpr inline bool isZero() const noexcept {
            return value.isZero();
        }

        constexpr inline bool isNegative() const noexcept {
            return value.isNegative();
        }

        constexpr inline bool upperBit() const noexcept {
            return value.upperBit();
        }

        constexpr inline Integer addOne() const noexcept {
            return value.addOne();
        }

        constexpr inline Integer subOne() const noexcept {
            return value.subOne();
        }

        template<size_t index, p_enable_if(index < size)>
        constexpr inline uint8_t getByte() const noexcept {
            return value.template getByte<index>();
        }

        template<size_t index, p_enable_if(index < size)>
        constexpr inline Integer& setByte(uint8_t v) noexcept {
            value.template setByte<index>(v);
            return *this;
        }

        template<size_t index, p_enable_if(index < size * 8)>
        constexpr inline bool getBit() const noexcept {
            return value.template getBit<index>();
        }

        template<size_t index, p_enable_if(index < size * 8)>
        constexpr inline Integer& setBit(bool v) noexcept {
            value.template setBit<index>(v);
            return *this;
        }

        constexpr inline static bool add_overflow(
                const Integer &v1, const Integer &v2, Integer &out) noexcept {
            return V::add_overflow(v1.value, v2.value, out.value);
        }

        constexpr inline static bool increment_overflow(Integer &value) noexcept {
            return V::increment_overflow(value.value);
        }

        constexpr inline static bool sub_overflow(
                const Integer &v1, const Integer &v2, Integer &out) noexcept {
            return V::sub_overflow(v1.value, v2.value, out.value);
        }

        constexpr inline static bool decrement_overflow(Integer &value) noexcept {
            return V::decrement_overflow(value.value);
        }

        constexpr inline static void leftShiftOneBit(
                Integer &value, bool bit) noexcept {
            V::leftShiftOneBit(value.value, bit);
        }

        constexpr inline size_t numberOfLeadingZeros() const noexcept {
            return value.numberOfLeadingZeros();
        }

        constexpr inline size_t numberOfTrailingZeros() const noexcept {
            return value.numberOfTrailingZeros();
        }

        constexpr inline Integer<size, true> s() const noexcept {
            return *this;
        }

        constexpr inline Integer<size, false> u() const noexcept {
            return *this;
        }

        constexpr inline const typename V::U& ulow() const noexcept {
            return value.low;
        }

        constexpr inline typename V::U& ulow() noexcept {
            return value.low;
        }

        constexpr inline const typename V::U& uhigh() const noexcept {
            return value.high;
        }

        constexpr inline typename V::U& uhigh() noexcept {
            return value.high;
        }

        constexpr inline const typename V::U& uvalue() const noexcept {
            return value.value;
        }

        constexpr inline typename V::U& uvalue() noexcept {
            return value.value;
        }

        template<typename T>
        constexpr inline Integer(p_bool_t<T> n) noexcept :
        value(typename V::U(n)) { }

        template<size_t size1, bool sig1, size_t size2, bool sig2,
        p_enable_if((p_getIntegerType(size) == pow2) &&
                (size1 <= size / 2) && (size2 <= size / 2))>
        constexpr inline Integer(const Integer<size1, sig1> &low,
                const Integer<size2, sig2> &high) noexcept : value(low, high) { }

        template<typename T1, typename T2,
        p_enable_if((p_getIntegerType(size) == pow2) &&
                (sizeof (T1) <= size / 2) && (sizeof (T2) <= size / 2))>
        constexpr inline Integer(const p_int_t<T1> low,
                const p_int_t<T2> high) noexcept : value(low, high) { }

        template<typename T, size_t size2, bool sig2,
        p_enable_if((p_getIntegerType(size) == pow2) &&
                (sizeof (T) <= size / 2) && (size2 <= size / 2))>
        constexpr inline Integer(const p_int_t<T> low,
                const Integer<size2, sig2> &high) noexcept : value(low, high) { }

        template<size_t size2, bool sig2, typename T,
        p_enable_if((p_getIntegerType(size) == pow2) &&
                (size2 <= size / 2) && (sizeof (T) <= size / 2))>
        constexpr inline Integer(const Integer<size2, sig2> &low,
                const p_int_t<T> high) noexcept : value(low, high) { }

    private:

        template<typename T, p_IType = p_getIntegerType(size)>
        struct tcast_h {
        };

        template<typename T>
        struct tcast_h<T, native> {

            constexpr inline static V tcast(const T n) noexcept {
                return V(n);
            }
        };

        template<typename T>
        struct tcast_h<T, pow2> {

            constexpr inline static V tcast(const T n) noexcept {
                return Integer(Integer<sizeof (T), JIO::p_is_signed<T>()>(n)).value;
            }
        };

    public:

        template<typename T, p_enable_if(size >= sizeof (T))>
        constexpr inline Integer(const p_int_t<T> n) noexcept :
        value(tcast_h<T>::tcast(n)) { }

        template<typename T, p_enable_if(size < sizeof (T))>
        constexpr explicit inline Integer(const p_int_t<T> n) noexcept :
        value(tcast_h<T>::tcast(n)) { }

        template<size_t size2, bool sig2, p_enable_if(size2 >= size)>
        constexpr inline operator Integer<size2, sig2>() const noexcept {
            return upcast<size2, sig2>();
        }

        template<size_t size2, bool sig2, p_enable_if(size2 < size)>
        constexpr explicit inline
        operator Integer<size2, sig2>() const noexcept {
            return downcast<size2, sig2>();
        }

    private:

        template<typename T,
        bool = (p_getIntegerType(size) == native) && (sizeof (T) >= size),
        bool = (p_getIntegerType(size) == native) && (sizeof (T) < size),
        bool = (p_getIntegerType(size) != native) && (sizeof (T) < size)>
        struct pcast_h {
        };

        template<typename T>
        struct pcast_h<T, true, false, false> {

            constexpr inline static T pcast(const Integer& v) noexcept {
                return T(p_castUS<typename V::U, typename V::S,
                        sig, typename V::U > (v.value.value));
            }
        };

        template<typename T>
        struct pcast_h<T, false, true, false> {

            constexpr inline static T pcast(const Integer& v) noexcept {
                return T(v.value.value);
            }
        };

        template<typename T>
        struct pcast_h<T, false, false, true> {

            constexpr inline static T pcast(const Integer& v) noexcept {
                return T(Integer<sizeof (T), JIO::p_is_signed<T>()>(v));
            }
        };

    public:

        template<typename T, p_enable_if(sizeof (T) >= size)>
        constexpr inline operator p_int_t<T>() const noexcept {
            return pcast_h<T>::pcast(*this);
        }

        template<typename T, p_enable_if(sizeof (T) < size)>
        constexpr explicit inline operator p_int_t<T>() const noexcept {
            return pcast_h<T>::pcast(*this);
        }

        template<typename T>
        constexpr inline operator p_bool_t<T>() const noexcept {
            return !(this->isZero());
        }

        template<size_t size2, bool sig2,
        typename R = result_t<size, size2, sig, sig2>>
        constexpr inline R
        operator+(const Integer<size2, sig2> &v2) const noexcept {
            return R(*this).value + R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = result_t<size, size2, sig, sig2>>
        constexpr inline R
        operator-(const Integer<size2, sig2> &v2) const noexcept {
            return R(*this).value - R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = result_t<size, size2, sig, sig2>>
        constexpr inline R
        operator*(const Integer<size2, sig2> &v2) const noexcept {
            return R(*this).value * R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = result_t<size, size2, sig, sig2>>
        constexpr inline R
        operator/(const Integer<size2, sig2> &v2) const noexcept {
            return typename R::V(R(*this).value / R(v2).value);
        }

        template<size_t size2, bool sig2,
        typename R = result_t<size, size2, sig, sig2>>
        constexpr inline R
        operator%(const Integer<size2, sig2> &v2) const noexcept {
            return typename R::V(R(*this).value % R(v2).value);
        }

        template<size_t size2, bool sig2,
        typename R = result_t<size, size2, sig, sig2>>
        constexpr inline R
        operator|(const Integer<size2, sig2> &v2) const noexcept {
            return R(*this).value | R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = result_t<size, size2, sig, sig2>>
        constexpr inline R
        operator&(const Integer<size2, sig2> &v2) const noexcept {
            return R(*this).value & R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = result_t<size, size2, sig, sig2>>
        constexpr inline R
        operator^(const Integer<size2, sig2> &v2) const noexcept {
            return R(*this).value ^ R(v2).value;
        }

        template<size_t size2, bool sig2>
        constexpr inline Integer
        operator<<(const Integer<size2, sig2> &v2) const noexcept {
            return value << p_SHType<size>(v2);
        }

        template<typename T>
        constexpr inline Integer
        operator<<(const p_int_t<T> v2) const noexcept {
            return value << p_SHType<size>(v2);
        }

        template<size_t size2, bool sig2>
        constexpr inline Integer
        operator>>(const Integer<size2, sig2> &v2) const noexcept {
            return V(value >> p_SHType<size>(v2));
        }

        template<typename T>
        constexpr inline Integer
        operator>>(const p_int_t<T> v2) const noexcept {
            return V(value >> p_SHType<size>(v2));
        }

        template<size_t size2, bool sig2,
        typename R = result_t<size, size2, sig, sig2>>
        constexpr inline bool
        operator==(const Integer<size2, sig2> &v2) const noexcept {
            return R(*this).value == R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = result_t<size, size2, sig, sig2>>
        constexpr inline bool
        operator!=(const Integer<size2, sig2> &v2) const noexcept {
            return R(*this).value != R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = result_t<size, size2, sig, sig2>>
        constexpr inline bool
        operator<=(const Integer<size2, sig2> &v2) const noexcept {
            return R(*this).value <= R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = result_t<size, size2, sig, sig2>>
        constexpr inline bool
        operator>=(const Integer<size2, sig2> &v2) const noexcept {
            return R(*this).value >= R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = result_t<size, size2, sig, sig2>>
        constexpr inline bool
        operator<(const Integer<size2, sig2> &v2) const noexcept {
            return R(*this).value < R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = result_t<size, size2, sig, sig2>>
        constexpr inline bool
        operator>(const Integer<size2, sig2> &v2) const noexcept {
            return R(*this).value > R(v2).value;
        }

        constexpr inline Integer operator~() const noexcept {
            return ~value;
        }

        constexpr inline Integer operator-() const noexcept {
            return -value;
        }

        constexpr inline Integer operator+() const noexcept {
            return *this;
        }

        constexpr inline Integer& operator++() noexcept {
            return *this = addOne();
        }

        constexpr inline Integer& operator--() noexcept {
            return *this = subOne();
        }

        constexpr inline Integer operator++(int) noexcept {
            Integer tmp = *this;
            operator++();
            return tmp;
        }

        constexpr inline Integer operator--(int) noexcept {
            Integer tmp = *this;
            operator--();
            return tmp;
        }

        template<size_t size2, bool sig2>
        friend class Integer;

        template<size_t size2, bool sig2>
        friend struct p_native_Integer_Impl;

        template<size_t size2, bool sig2>
        friend class p_pow2_Integer_Impl;
    };

    namespace p_i_print {

        template<size_t size, size_t... index,
        typename R = Integer<size, false >>
        constexpr inline R
        div_ten_const_h(p_i_seq::array_t<size_t, index...>) noexcept {
            R out = R::ZERO();
            p_i_seq::unused_array<size>({
                (out.template setByte<index>(index == 0 ? 0xcd : 0xcc))...
            });
            return out;
        }

        template<size_t size>
        constexpr inline Integer<size, false> div_ten_const() noexcept {
            return div_ten_const_h<size>(p_i_seq::make_array<size_t, 0, size>());
        }

        template<size_t size, p_enable_if(p_getIntegerType(size) == native)>
        constexpr inline unsigned
        divremTen(Integer<size, false> &v) noexcept {
            unsigned tmp = unsigned(v);
            v /= Integer<size, false>(10);
            return tmp - unsigned(v) * 10;
        }

        template<size_t size, p_enable_if(p_getIntegerType(size) == pow2)>
        constexpr inline unsigned
        divremTen(Integer<size, false> &v) noexcept {
            constexpr Integer<size, false> dc = div_ten_const<size>();
            unsigned tmp = unsigned(v);
            v = wmultiply(v, dc).uhigh() >> 3;
            return tmp - unsigned(v) * 10;
        }

        constexpr inline size_t p_digits10(size_t size, bool sig) noexcept {
            static_assert(sizeof (size_t) <= 8, "too big size_t");
            return size_t(wmultiply<8>(size * 8 - sig,
                    0x4d104d427de7FbccULL).uhigh());
        }

        template<size_t size, bool sig, typename CharT, typename Traits>
        inline void printDec(const Integer<size, sig> &v,
                std::basic_ostream<CharT, Traits> &out) {
            auto f = out.flags();
            constexpr size_t d = p_digits10(size, sig) + 2;
            bool neg = false;
            Integer<size, false> tmp = v;
            if (v.isNegative()) {
                tmp = -tmp;
                neg = true;
            }
            char str[d + 1]; // trailing \0
            size_t i = d;
            str[i--] = 0;
            while (tmp >= 10) {
                str[i--] = p_i_utils::digits[divremTen(tmp)];
            }
            str[i] = p_i_utils::digits[unsigned(tmp)];
            if (neg) {
                str[--i] = '-';
            } else if (f & std::ios_base::showpos) {
                str[--i] = '+';
            }
            out << &str[i];
        }

        template<size_t size, bool sig, typename CharT, typename Traits>
        inline void printHex(const Integer<size, sig> &v,
                std::basic_ostream<CharT, Traits> &out) {
            auto f = out.flags();
            constexpr size_t d = size * 2 + 2;
            Integer<size, false> tmp = v;
            char str[d + 1]; // trailing \0
            size_t i = d;
            str[i--] = 0;
            while (tmp >= 16) {
                str[i--] = p_i_utils::digits[unsigned(tmp) & 0xf];
                tmp >>= 4;
            }
            str[i] = p_i_utils::digits[unsigned(tmp)];
            if (f & std::ios_base::showbase) {
                str[--i] = 'x';
                str[--i] = '0';
            }
            out << &str[i];
        }

        template<size_t size, bool sig, typename CharT, typename Traits>
        inline void printOct(const Integer<size, sig> &v,
                std::basic_ostream<CharT, Traits> &out) {
            auto f = out.flags();
            constexpr size_t d = (size * 8 + 2) / 3 + 1;
            Integer<size, false> tmp = v;
            char str[d + 1]; // trailing \0
            size_t i = d;
            str[i--] = 0;
            while (tmp >= 8) {
                str[i--] = p_i_utils::digits[unsigned(tmp) & 0x7];
                tmp >>= 3;
            }
            str[i] = p_i_utils::digits[unsigned(tmp)];
            if (f & std::ios_base::showbase) {
                str[--i] = '0';
            }
            out << &str[i];
        }

        template<size_t size, bool sig, typename CharT, typename Traits>
        inline void print(const Integer<size, sig> &v,
                std::basic_ostream<CharT, Traits> &out) {
            auto f = out.flags();
            if (f & std::ios_base::hex) {
                printHex(v, out);
                return;
            }
            if (f & std::ios_base::oct) {
                printOct(v, out);
                return;
            }
            printDec(v, out);
        }
    }

    template<size_t size, bool sig, typename CharT, typename Traits>
    inline std::basic_ostream<CharT, Traits>&
    operator<<(std::basic_ostream<CharT, Traits> &out,
            const Integer<size, sig> &v) {
        Integer < p_i_utils::make_pow2(size), sig> tmp = v;
        p_i_print::print(tmp, out);
        return out;
    }

    template<size_t size1, bool sig1, typename T,
    typename R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>>
    constexpr inline R
    operator+(const Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return R(v1) + R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>>
    constexpr inline R
    operator+(const p_int_t<T> v1, const Integer<size1, sig1> &v2) noexcept {
        return R(v1) + R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>&
    operator+=(Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) noexcept {
        return v1 = (v1 + Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if(sizeof (T) <= size1)>
    constexpr inline Integer<size1, sig1>&
    operator+=(Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return v1 = (v1 + Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if(size1 <= sizeof (T))>
    constexpr inline T&
    operator+=(p_int_t<T> &v1, const Integer<size1, sig1> &v2) noexcept {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) + R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>>
    constexpr inline R
    operator-(const Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return R(v1) - R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>>
    constexpr inline R
    operator-(const p_int_t<T> v1, const Integer<size1, sig1> &v2) noexcept {
        return R(v1) - R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>&
    operator-=(Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) noexcept {
        return v1 = (v1 - Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if(sizeof (T) <= size1)>
    constexpr inline Integer<size1, sig1>&
    operator-=(Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return v1 = (v1 - Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if(size1 <= sizeof (T))>
    constexpr inline T&
    operator-=(p_int_t<T> &v1, const Integer<size1, sig1> &v2) noexcept {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) - R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>>
    constexpr inline R
    operator*(const Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return R(v1) * R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>>
    constexpr inline R
    operator*(const p_int_t<T> v1, const Integer<size1, sig1> &v2) noexcept {
        return R(v1) * R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>&
    operator*=(Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) noexcept {
        return v1 = (v1 * Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if(sizeof (T) <= size1)>
    constexpr inline Integer<size1, sig1>&
    operator*=(Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return v1 = (v1 * Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if(size1 <= sizeof (T))>
    constexpr inline T&
    operator*=(p_int_t<T> &v1, const Integer<size1, sig1> &v2) noexcept {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) * R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>>
    constexpr inline R
    operator/(const Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return R(v1) / R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>>
    constexpr inline R
    operator/(const p_int_t<T> v1, const Integer<size1, sig1> &v2) noexcept {
        return R(v1) / R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>&
    operator/=(Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) noexcept {
        return v1 = (v1 / Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if(sizeof (T) <= size1)>
    constexpr inline Integer<size1, sig1>&
    operator/=(Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return v1 = (v1 / Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if(size1 <= sizeof (T))>
    constexpr inline T&
    operator/=(p_int_t<T> &v1, const Integer<size1, sig1> &v2) noexcept {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) / R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>>
    constexpr inline R
    operator%(const Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return R(v1) % R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>>
    constexpr inline R
    operator%(const p_int_t<T> v1, const Integer<size1, sig1> &v2) noexcept {
        return R(v1) % R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>&
    operator%=(Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) noexcept {
        return v1 = (v1 % Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if(sizeof (T) <= size1)>
    constexpr inline Integer<size1, sig1>&
    operator%=(Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return v1 = (v1 % Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if(size1 <= sizeof (T))>
    constexpr inline T&
    operator%=(p_int_t<T> &v1, const Integer<size1, sig1> &v2) noexcept {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) % R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>>
    constexpr inline R
    operator|(const Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return R(v1) | R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>>
    constexpr inline R
    operator|(const p_int_t<T> v1, const Integer<size1, sig1> &v2) noexcept {
        return R(v1) | R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>&
    operator|=(Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) noexcept {
        return v1 = (v1 | Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if(sizeof (T) <= size1)>
    constexpr inline Integer<size1, sig1>&
    operator|=(Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return v1 = (v1 | Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if(size1 <= sizeof (T))>
    constexpr inline T&
    operator|=(p_int_t<T> &v1, const Integer<size1, sig1> &v2) noexcept {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) | R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>>
    constexpr inline R
    operator&(const Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return R(v1) & R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>>
    constexpr inline R
    operator&(const p_int_t<T> v1, const Integer<size1, sig1> &v2) noexcept {
        return R(v1) & R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>&
    operator&=(Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) noexcept {
        return v1 = (v1 & Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if(sizeof (T) <= size1)>
    constexpr inline Integer<size1, sig1>&
    operator&=(Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return v1 = (v1 & Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if(size1 <= sizeof (T))>
    constexpr inline T&
    operator&=(p_int_t<T> &v1, const Integer<size1, sig1> &v2) noexcept {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) & R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>>
    constexpr inline R
    operator^(const Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return R(v1) ^ R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>>
    constexpr inline R
    operator^(const p_int_t<T> v1, const Integer<size1, sig1> &v2) noexcept {
        return R(v1) ^ R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>&
    operator^=(Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) noexcept {
        return v1 = (v1 ^ Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if(sizeof (T) <= size1)>
    constexpr inline Integer<size1, sig1>&
    operator^=(Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return v1 = (v1 ^ Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if(size1 <= sizeof (T))>
    constexpr inline T&
    operator^=(p_int_t<T> &v1, const Integer<size1, sig1> &v2) noexcept {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) ^ R(v2));
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline bool
    operator==(const Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        using R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>;
        return R(v1) == R(v2);
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline bool
    operator==(const p_int_t<T> v1, const Integer<size1, sig1> &v2) noexcept {
        using R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>;
        return R(v1) == R(v2);
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline bool
    operator!=(const Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        using R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>;
        return R(v1) != R(v2);
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline bool
    operator!=(const p_int_t<T> v1, const Integer<size1, sig1> &v2) noexcept {
        using R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>;
        return R(v1) != R(v2);
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline bool
    operator<=(const Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        using R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>;
        return R(v1) <= R(v2);
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline bool
    operator<=(const p_int_t<T> v1, const Integer<size1, sig1> &v2) noexcept {
        using R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>;
        return R(v1) <= R(v2);
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline bool
    operator>=(const Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        using R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>;
        return R(v1) >= R(v2);
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline bool
    operator>=(const p_int_t<T> v1, const Integer<size1, sig1> &v2) noexcept {
        using R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>;
        return R(v1) >= R(v2);
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline bool
    operator<(const Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        using R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>;
        return R(v1) < R(v2);
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline bool
    operator<(const p_int_t<T> v1, const Integer<size1, sig1> &v2) noexcept {
        using R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>;
        return R(v1) < R(v2);
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline bool
    operator>(const Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        using R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>;
        return R(v1) > R(v2);
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline bool
    operator>(const p_int_t<T> v1, const Integer<size1, sig1> &v2) noexcept {
        using R = result_t<size1, sizeof (T), sig1, p_is_signed<T>()>;
        return R(v1) > R(v2);
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline T
    operator<<(const p_int_t<T> v1, const Integer<size1, sig1> &v2) noexcept {
        using R = Integer <sizeof (T), (p_is_signed<T>())>;
        return T(R(v1) << v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    constexpr inline Integer<size1, sig1>&
    operator<<=(Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) noexcept {
        return v1 = (v1 << v2);
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline Integer<size1, sig1>&
    operator<<=(Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return v1 = (v1 << v2);
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline T&
    operator<<=(p_int_t<T> &v1, const Integer<size1, sig1> &v2) noexcept {
        return v1 = T(Integer<sizeof (T), p_is_signed<T>()>(v1) << v2);
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline T
    operator>>(const p_int_t<T> v1, const Integer<size1, sig1> &v2) noexcept {
        using R = Integer <sizeof (T), (p_is_signed<T>())>;
        return T(R(v1) >> v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    constexpr inline Integer<size1, sig1>&
    operator>>=(Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) noexcept {
        return v1 = (v1 >> v2);
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline Integer<size1, sig1>&
    operator>>=(Integer<size1, sig1> &v1, const p_int_t<T> v2) noexcept {
        return v1 = (v1 >> v2);
    }

    template<size_t size1, bool sig1, typename T>
    constexpr inline T&
    operator>>=(p_int_t<T> &v1, const Integer<size1, sig1> &v2) noexcept {
        return v1 = T(Integer<sizeof (T), p_is_signed<T>()>(v1) >> v2);
    }

    namespace p_literal {
        template <typename T, T... values>
        using array_t = p_i_seq::array_t<T, values...>;

        constexpr size_t bits_to_bytes(size_t bits, bool sig) noexcept {
            return p_i_utils::make_pow2((bits + 7 + sig) / 8);
        }

        template<bool sig, int bit_per_symbol, int first, int... nums>
        constexpr inline auto get_bytes() noexcept {
            return bits_to_bytes(sizeof...(nums) * bit_per_symbol +
                    (4 - p_i_utils::numberOfLeadingZeros4Bit_n(first)), sig);
        }

        template<bool sig, int... nums,
        size_t bytes = get_bytes<sig, 4, nums...>()>
        constexpr inline auto parseHex(array_t<int, nums...> arr) noexcept {
            using I = Integer<bytes, sig>;
            I out = I::ZERO();
            for (size_t i = 0; i < sizeof...(nums); i++) {
                out <<= 4;
                out |= I(arr[i]);
            }
            return out;
        }

        template<bool sig>
        constexpr inline auto parseHex(array_t<int>) noexcept {
            return Integer<1, sig>::ZERO();
        }

        template<bool sig, size_t bytes, int... nums>
        constexpr inline auto parseDec_h(array_t<int, nums...> arr) noexcept {
            using I = Integer<bytes, sig>;
            I out = I::ZERO();
            for (size_t i = 0; i < sizeof...(nums); i++) {
                out *= I(10);
                out += I(arr[i]);
            }
            return out;
        }

        template<bool sig, int... nums,
        size_t hex_bytes = bits_to_bytes(sizeof...(nums) * 4, false),
        size_t bytes = bits_to_bytes(
                hex_bytes * 8 - parseDec_h<false, hex_bytes>(
                array_t<int, nums...>()).numberOfLeadingZeros(), sig)>
        constexpr inline auto parseDec(array_t<int, nums...> arr) noexcept {
            return parseDec_h<sig, bytes>(arr);
        }

        template<bool sig, int... nums,
        size_t bytes = get_bytes<sig, 3, nums...>()>
        constexpr inline auto parseOct(array_t<int, nums...> arr) noexcept {
            using I = Integer<bytes, sig>;
            I out = I::ZERO();
            for (size_t i = 0; i < sizeof...(nums); i++) {
                out <<= 3;
                out |= I(arr[i]);
            }
            return out;
        }

        template<bool sig>
        constexpr inline auto parseOct(array_t<int>) noexcept {
            return Integer<1, sig>::ZERO();
        }

        template<bool sig, int... nums,
        size_t bytes = get_bytes<sig, 1, nums...>()>
        constexpr inline auto parseBin(array_t<int, nums...> arr) noexcept {
            using I = Integer<bytes, sig>;
            I out = I::ZERO();
            for (size_t i = 0; i < sizeof...(nums); i++) {
                out <<= 1;
                out |= I(arr[i]);
            }
            return out;
        }

        template<bool sig>
        constexpr inline auto parseBin(array_t<int>) noexcept {
            return Integer<1, sig>::ZERO();
        }

        template<int min, int max, int value,
        p_enable_if((value >= min) && (value <= max))>
        constexpr inline int contains() noexcept {
            return value;
        }

        template<int min, int max, char... chars>
        constexpr inline auto verify() noexcept {
            return array_t<int, contains<min, max,
                    p_i_utils::indexOfDigit<chars>()>()...>();
        }

        template<int min, int max>
        constexpr inline auto remove_zeros_and_verify() noexcept {
            return array_t<int>();
        }

        template<int min, int max, char a, char... chars, p_enable_if(a != '0')>
        constexpr inline auto remove_zeros_and_verify() noexcept {
            return verify<min, max, a, chars...>();
        }

        template<int min, int max, char a, char... chars, p_enable_if(a == '0')>
        constexpr inline auto remove_zeros_and_verify() noexcept {
            return remove_zeros_and_verify<min, max, chars...>();
        }

        template<bool sig, char a, char b, char... chars,
        p_enable_if((a == '0') && !(b == 'b' || b == 'B' || b == 'x' || b == 'X'))>
        constexpr inline auto parse() noexcept {
            return parseOct<sig>(remove_zeros_and_verify<0, 7, b, chars...>());
        }

        template<bool sig, char a, char b, char... chars,
        p_enable_if((a == '0') && (b == 'b' || b == 'B'))>
        constexpr inline auto parse() noexcept {
            return parseBin<sig>(remove_zeros_and_verify<0, 1, chars...>());
        }

        template<bool sig, char a, char b, char... chars,
        p_enable_if((a == '0') && (b == 'x' || b == 'X'))>
        constexpr inline auto parse() noexcept {
            return parseHex<sig>(remove_zeros_and_verify<0, 15, chars...>());
        }

        template<bool sig, char a, char... chars, p_enable_if(a != '0')>
        constexpr inline auto parse() noexcept {
            return parseDec<sig>(remove_zeros_and_verify<0, 10, a, chars...>());
        }

        template<bool sig, char a, p_enable_if(a == '0')>
        constexpr inline auto parse() noexcept {
            return Integer<1, sig>(0);
        }
    }

    template<char... chars>
    constexpr inline auto operator""_UI() noexcept {
        return p_literal::parse<false, chars...>();
    }

    template<char... chars>
    constexpr inline auto operator""_SI() noexcept {
        return p_literal::parse<true, chars...>();
    }

#undef p_enable_if
}

#endif /* INTEGER_HPP */

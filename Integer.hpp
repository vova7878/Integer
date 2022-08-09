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
#include <tuple>
#include <cstddef>
#include <stdexcept>
#include <iostream>

// -std=c++14
namespace JIO {

    template<bool A>
    using p_enable_if_t = typename std::enable_if<A, bool>::type;

#define p_enable_if(B) p_enable_if_t<(B)> = false

    template<typename T>
    constexpr inline T lowestOneBit(const T i) {
        // HD, Section 2-1
        return i & -i;
    }

    template<typename T>
    constexpr inline bool isOneBit(const T i) {
        return lowestOneBit(i) == i;
    }

    template<typename T>
    constexpr const T& max(const T &v1, const T &v2) {
        return v1 < v2 ? v2 : v1;
    }

    template<typename T>
    constexpr const T& min(const T &v1, const T &v2) {
        return v1 < v2 ? v1 : v2;
    }

    constexpr inline size_t p_numberOfLeadingZeros2Bit_h(uint8_t i) {
        return i ? (1 - (i >> 1)) : 2;
    }

    constexpr inline size_t p_numberOfLeadingZeros4Bit_h(uint8_t i) {
        return (i >= 1 << 2) ?
                p_numberOfLeadingZeros2Bit_h(i >> 2) :
                p_numberOfLeadingZeros2Bit_h(i) + 2;
    }

    constexpr inline size_t p_numberOfLeadingZeros_h(uint8_t i) {
        return (i >= 1 << 4) ?
                p_numberOfLeadingZeros4Bit_h(i >> 4) :
                p_numberOfLeadingZeros4Bit_h(i) + 4;
    }

    constexpr inline size_t p_numberOfLeadingZeros_h(uint16_t i) {
        return (i >= 1 << 8) ?
                p_numberOfLeadingZeros_h(uint8_t(i >> 8)) :
                p_numberOfLeadingZeros_h(uint8_t(i)) + 8;
    }

    constexpr inline size_t p_numberOfLeadingZeros_h(uint32_t i) {
        return (i >= 1 << 16) ?
                p_numberOfLeadingZeros_h(uint16_t(i >> 16)) :
                p_numberOfLeadingZeros_h(uint16_t(i)) + 16;
    }

    constexpr inline size_t p_numberOfLeadingZeros_h(uint64_t i) {
        return (i >= uint64_t(1) << 32) ?
                p_numberOfLeadingZeros_h(uint32_t(i >> 32)) :
                p_numberOfLeadingZeros_h(uint32_t(i)) + 32;
    }

    constexpr inline size_t p_numberOfTrailingZeros2Bit_h(uint8_t i) {
        return i ? (1 - (i & 1)) : 2;
    }

    constexpr inline size_t p_numberOfTrailingZeros4Bit_h(uint8_t i) {
        return (i & 0x3) ?
                p_numberOfTrailingZeros2Bit_h(i) :
                p_numberOfTrailingZeros2Bit_h(i >> 2) + 2;
    }

    constexpr inline size_t p_numberOfTrailingZeros_h(uint8_t i) {
        return (i & 0xf) ?
                p_numberOfTrailingZeros4Bit_h(i) :
                p_numberOfTrailingZeros4Bit_h(i >> 4) + 4;
    }

    constexpr inline size_t p_numberOfTrailingZeros_h(uint16_t i) {
        return (uint8_t(i)) ?
                p_numberOfTrailingZeros_h(uint8_t(i)) :
                p_numberOfTrailingZeros_h(uint8_t(i >> 8)) + 8;
    }

    constexpr inline size_t p_numberOfTrailingZeros_h(uint32_t i) {
        return (uint16_t(i)) ?
                p_numberOfTrailingZeros_h(uint16_t(i)) :
                p_numberOfTrailingZeros_h(uint16_t(i >> 16)) + 16;
    }

    constexpr inline size_t p_numberOfTrailingZeros_h(uint64_t i) {
        return (uint32_t(i)) ?
                p_numberOfTrailingZeros_h(uint32_t(i)) :
                p_numberOfTrailingZeros_h(uint32_t(i >> 32)) + 32;
    }

    enum p_IType {
        illegal = 0,
        native,
        pow2,
        array
    };

    constexpr p_IType p_getIntegerType(size_t size) {
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

    template<typename T>
    struct p_Operators_Impl;

    template<typename U, typename S>
    class p_Integer_S;

    template<typename UT, typename ST>
    class p_Integer_U {
    private:
        typedef ST S;
        typedef UT U;
        typedef p_Integer_U I;
        typedef p_SHType<sizeof (U) > M;
        constexpr static M shmask = sizeof (U) * 8 - 1;
        U value;
    public:

        constexpr inline p_Integer_U() = default;

        constexpr explicit inline p_Integer_U(const U n) : value(n) { }

        constexpr inline bool isNegative() const {
            return false;
        };

        constexpr inline static bool add_overflow(
                const I &v1, const I &v2, I &out) {
            U tmp = v1.value + v2.value;
            out = I(tmp);
            return tmp < v1.value;
        }

        constexpr inline static bool increment_overflow(I &value) {
            return (++value.value) == 0;
        }

        constexpr inline static bool sub_overflow(
                const I &v1, const I &v2, I &out) {
            U tmp = v1.value - v2.value;
            out = I(tmp);
            return tmp > v1.value;
        }

        constexpr inline static bool decrement_overflow(I &value) {
            return (value.value--) == 0;
        }

        constexpr inline I operator/(const I &other) const {
            if (other.value == 0) {
                throw std::runtime_error("Division by zero");
            }
            return p_Integer_U(value / other.value);
        }

        constexpr inline I operator%(const I &other) const {
            if (other.value == 0) {
                throw std::runtime_error("Division by zero");
            }
            return p_Integer_U(value % other.value);
        }

        constexpr inline I operator>>(const M other) const {
            return p_Integer_U(value >> (other & shmask));
        }

        constexpr inline bool operator>(const I &other) const {
            return value > other.value;
        }

        constexpr inline bool operator<(const I &other) const {
            return value < other.value;
        }

        constexpr inline bool operator>=(const I &other) const {
            return value >= other.value;
        }

        constexpr inline bool operator<=(const I &other) const {
            return value <= other.value;
        }

        template<size_t size, bool sig>
        friend class Integer;

        friend p_Operators_Impl<p_Integer_U<U, S>>;
    };

    template<typename UT, typename ST>
    class p_Integer_S {
    private:
        typedef ST S;
        typedef UT U;
        typedef p_Integer_S I;
        typedef p_SHType<sizeof (U) > M;
        constexpr static M shmask = sizeof (U) * 8 - 1;
        U value;
    public:

        constexpr inline p_Integer_S() = default;

        constexpr explicit inline p_Integer_S(const S n) : value(n) { }

        constexpr inline bool isNegative() const {
            return S(value) < 0;
        };

        template<size_t size, size_t = size>
        struct divrem_h {

            constexpr inline static I div(const I &a, const I &b) {
                if (b.value == 0) {
                    throw std::runtime_error("Division by zero");
                }
                return I(S(a.value) / S(b.value));
            }

            constexpr inline static I rem(const I &a, const I &b) {
                if (b.value == 0) {
                    throw std::runtime_error("Division by zero");
                }
                return I(S(a.value) % S(b.value));
            }
        };

        template<size_t size>
        struct divrem_h<size, 4> {

            constexpr inline static I div(const I &a, const I &b) {
                if ((a.value == U(0x80000000L)) && (b.value == U(-1L))) {
                    return I(0x80000000L);
                }
                return divrem_h<size, 0>::div(a, b);
            }

            constexpr inline static I rem(const I &a, const I &b) {
                if ((a.value == U(0x80000000L)) && (b.value == U(-1L))) {
                    return I(0);
                }
                return divrem_h<size, 0>::rem(a, b);
            }
        };

        template<size_t size>
        struct divrem_h<size, 8> {

            constexpr inline static I div(const I &a, const I &b) {
                if ((a.value == U(0x8000000000000000LL)) && (b.value == U(-1LL))) {
                    return I(0x8000000000000000LL);
                }
                return divrem_h<size, 0>::div(a, b);
            }

            constexpr inline static I rem(const I &a, const I &b) {
                if ((a.value == U(0x8000000000000000LL)) && (b.value == U(-1LL))) {
                    return I(0);
                }
                return divrem_h<size, 0>::rem(a, b);
            }
        };

        constexpr inline I operator/(const I &other) const {
            return divrem_h<sizeof (U)>::div(*this, other);
        }

        constexpr inline I operator%(const I &other) const {
            return divrem_h<sizeof (U)>::rem(*this, other);
        }

        constexpr inline I operator>>(const M other) const {
            return I(S(value) >> (other & shmask));
        }

        constexpr inline bool operator>(const I &other) const {
            return S(value) > S(other.value);
        }

        constexpr inline bool operator<(const I &other) const {
            return S(value) < S(other.value);
        }

        constexpr inline bool operator>=(const I &other) const {
            return S(value) >= S(other.value);
        }

        constexpr inline bool operator<=(const I &other) const {
            return S(value) <= S(other.value);
        }

        template<size_t size, bool sig>
        friend class Integer;

        friend p_Operators_Impl<p_Integer_S<U, S>>;
    };

    template<typename T>
    struct p_Operators_Impl : public T {
    private:

        typedef p_Operators_Impl I;
        typedef typename T::U U;
        typedef typename T::S S;

    public:

        using T::T;

        constexpr inline p_Operators_Impl() = default;

        constexpr inline p_Operators_Impl(const T &obj) : T(obj) { }

        void printv(std::ostream &out) {
            out << std::hex << uint64_t(T::value) << std::dec;
        }

        constexpr inline static I ZERO() {
            return T(0);
        };

        constexpr inline bool isZero() const {
            return T::value == 0;
        };

        constexpr inline bool upperBit() const {
            return S(T::value) < 0;
        };

        constexpr inline static bool add_overflow(
                const I &v1, const I &v2, I &out) {
            return T::add_overflow(v1, v2, out);
        }

        constexpr inline static bool increment_overflow(I &value) {
            return T::increment_overflow(value);
        }

        constexpr inline static bool sub_overflow(
                const I &v1, const I &v2, I &out) {
            return T::sub_overflow(v1, v2, out);
        }

        constexpr inline static bool decrement_overflow(I &value) {
            return T::decrement_overflow(value);
        }

        constexpr inline static void leftShiftOneBit(I &value, bool bit) {
            value.value = (value.value << 1) | U(bit);
        }

        constexpr inline size_t numberOfLeadingZeros() const {
            return p_numberOfLeadingZeros_h(T::value);
        }

        constexpr inline size_t numberOfTrailingZeros() const {
            return p_numberOfTrailingZeros_h(T::value);
        }

        constexpr inline I operator+() const {
            return *this;
        }

        constexpr inline I operator-() const {
            return I(-T::value);
        }

        constexpr inline I operator+(const I &other) const {
            return I(T::value + other.value);
        }

        constexpr inline I operator-(const I &other) const {
            return I(T::value - other.value);
        }

        constexpr inline I operator*(const I &other) const {
            return I(T::value * other.value);
        }

        constexpr inline I operator|(const I &other) const {
            return I(T::value | other.value);
        }

        constexpr inline I operator&(const I &other) const {
            return I(T::value & other.value);
        }

        constexpr inline I operator^(const I &other) const {
            return I(T::value ^ other.value);
        }

        constexpr inline I operator<<(const typename T::M other) const {
            return I(T::value << (other & T::shmask));
        }

        constexpr inline bool operator==(const I &other) const {
            return T::value == other.value;
        }

        constexpr inline bool operator!=(const I &other) const {
            return T::value != other.value;
        }

        constexpr inline I operator~() const {
            return I(~T::value);
        }

    private:

        constexpr inline I p1() const {
            return I(T::value + 1);
        }

        constexpr inline I m1() const {
            return I(T::value - 1);
        }

        template<size_t size, bool sig>
        friend class Integer;
    };

    template<>
    struct p_Integer_Impl<1, false> {
        typedef p_Operators_Impl<p_Integer_U<uint8_t, int8_t>> type;
    };

    template<>
    struct p_Integer_Impl<1, true> {
        typedef p_Operators_Impl<p_Integer_S<uint8_t, int8_t>> type;
    };

    template<>
    struct p_Integer_Impl<2, false> {
        typedef p_Operators_Impl<p_Integer_U<uint16_t, int16_t>> type;
    };

    template<>
    struct p_Integer_Impl<2, true> {
        typedef p_Operators_Impl<p_Integer_S<uint16_t, int16_t>> type;
    };

    template<>
    struct p_Integer_Impl<4, false> {
        typedef p_Operators_Impl<p_Integer_U<uint32_t, int32_t>> type;
    };

    template<>
    struct p_Integer_Impl<4, true> {
        typedef p_Operators_Impl<p_Integer_S<uint32_t, int32_t>> type;
    };

    template<>
    struct p_Integer_Impl<8, false> {
        typedef p_Operators_Impl<p_Integer_U<uint64_t, int64_t>> type;
    };

    template<>
    struct p_Integer_Impl<8, true> {
        typedef p_Operators_Impl<p_Integer_S<uint64_t, int64_t>> type;
    };

    template<size_t half, bool sig>
    class p_Pow2_Integer_Impl;

    template<size_t size, bool sig>
    struct p_Integer_Impl <size, sig, p_IType::pow2> {
        typedef p_Pow2_Integer_Impl<size / 2, sig> type;
    };

    template<size_t half, bool sig>
    class p_Pow2_Integer_Base;

    template<size_t half>
    class p_Pow2_Integer_Base<half, false> {
    private:
        typedef Integer<half, true> S;
        typedef Integer<half, false> U;
        typedef p_Pow2_Integer_Base<half, false> I;
        typedef p_SHType<half * 2> M;
        constexpr static M shmask = half * 2 * 8 - 1;
        U low, high;

        constexpr inline static I rightShift(
                const I &value, const M shiftDistance) {
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

        constexpr inline p_Pow2_Integer_Base() = default;

        constexpr explicit inline p_Pow2_Integer_Base(const U &low) :
        low(low), high(U::ZERO()) { }

        constexpr explicit inline p_Pow2_Integer_Base(const S &low) :
        low(low), high(low.upperBit() ? ~U::ZERO() : U::ZERO()) { }

        constexpr inline p_Pow2_Integer_Base(const U &low, const U &high) :
        low(low), high(high) { }

        constexpr inline bool isNegative() const {
            return false;
        };

        constexpr inline static bool add_overflow(
                const I &v1, const I &v2, I &out) {
            bool tmpo = U::add_overflow(v1.high, v2.high, out.high);
            if (U::add_overflow(v1.low, v2.low, out.low)) {
                return U::increment_overflow(out.high) || tmpo;
            }
            return tmpo;
        }

        constexpr inline static bool increment_overflow(I &value) {
            if (U::increment_overflow(value.low)) {
                return U::increment_overflow(value.high);
            }
            return false;
        }

        constexpr inline static bool sub_overflow(
                const I &v1, const I &v2, I &out) {
            bool tmpo = U::sub_overflow(v1.high, v2.high, out.high);
            if (U::sub_overflow(v1.low, v2.low, out.low)) {
                return U::decrement_overflow(out.high) || tmpo;
            }
            return tmpo;
        }

        constexpr inline static bool decrement_overflow(I &value) {
            if (U::decrement_overflow(value.low)) {
                return U::decrement_overflow(value.high);
            }
            return false;
        }

        constexpr inline bool operator>(const I &other) const {
            return (high > other.high) ? true :
                    ((high < other.high) ? false :
                    (low > other.low));
        }

        constexpr inline bool operator<(const I &other) const {
            return (high < other.high) ? true :
                    ((high > other.high) ? false :
                    (low < other.low));
        }

        constexpr inline bool operator>=(const I &other) const {
            return (high > other.high) ? true :
                    ((high < other.high) ? false :
                    (low >= other.low));
        }

        constexpr inline bool operator<=(const I &other) const {
            return (high < other.high) ? true :
                    ((high > other.high) ? false :
                    (low <= other.low));
        }

        constexpr inline I operator>>(const M other) const {
            return rightShift(*this, other & shmask);
        }

        template<size_t size2, bool sig2>
        friend class p_Pow2_Integer_Impl;

        template<size_t size2, bool sig2>
        friend class Integer;
    };

    template<size_t half>
    class p_Pow2_Integer_Base<half, true> {
    private:
        typedef Integer<half, true> S;
        typedef Integer<half, false> U;
        typedef p_Pow2_Integer_Base<half, true> I;
        typedef p_SHType<half * 2> M;
        constexpr static M shmask = half * 2 * 8 - 1;
        U low, high;

        inline static I rightShift(
                const I &value, const M shiftDistance) {
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

        constexpr inline p_Pow2_Integer_Base() = default;

        constexpr explicit inline p_Pow2_Integer_Base(const U &low) :
        low(low), high(U::ZERO()) { }

        constexpr explicit inline p_Pow2_Integer_Base(const S &low) :
        low(low), high(low.isNegative() ? ~U::ZERO() : U::ZERO()) { }

        constexpr inline p_Pow2_Integer_Base(const U &low, const U &high) :
        low(low), high(high) { }

        constexpr inline bool isNegative() const {
            return high.upperBit();
        };

        constexpr inline bool operator>(const I &other) const {
            return (S(high) > S(other.high)) ? true :
                    ((S(high) < S(other.high)) ? false :
                    (low > other.low));
        }

        constexpr inline bool operator<(const I &other) const {
            return (S(high) < S(other.high)) ? true :
                    ((S(high) > S(other.high)) ? false :
                    (low < other.low));
        }

        constexpr inline bool operator>=(const I &other) const {
            return (S(high) > S(other.high)) ? true :
                    ((S(high) < S(other.high)) ? false :
                    (low >= other.low));
        }

        constexpr inline bool operator<=(const I &other) const {
            return (S(high) < S(other.high)) ? true :
                    ((S(high) > S(other.high)) ? false :
                    (low <= other.low));
        }

        constexpr inline I operator>>(const M other) const {
            return rightShift(*this, other & shmask);
        }

        template<size_t size2, bool sig2>
        friend class p_Pow2_Integer_Impl;

        template<size_t size2, bool sig2>
        friend class Integer;
    };

    template<size_t half, bool sig>
    class p_Pow2_Integer_Impl : public p_Pow2_Integer_Base<half, sig> {
    private:
        typedef p_Pow2_Integer_Base<half, sig> T;
        typedef p_Pow2_Integer_Impl<half, sig> I;
        typedef p_Pow2_Integer_Impl<half, false> UI;
        typedef p_Pow2_Integer_Impl<half, true> SI;
        typedef typename T::S S;
        typedef typename T::U U;
        typedef typename T::M M;

        constexpr inline static I leftShift(const I &value,
                const typename T::M shiftDistance) {
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

        constexpr inline I p1() const {
            I tmp = *this;
            if (U::increment_overflow(tmp.low)) {
                ++tmp.high;
            }
            return tmp;
        }

        constexpr inline I m1() const {
            I tmp = *this;
            if (U::decrement_overflow(tmp.low)) {
                --tmp.high;
            }
            return tmp;
        }
    public:
        using T::T;

        constexpr inline p_Pow2_Integer_Impl() = default;

        template<bool sig2>
        constexpr inline p_Pow2_Integer_Impl(
                const p_Pow2_Integer_Impl<half, sig2> &other) :
        T(other.low, other.high) { }

        constexpr inline p_Pow2_Integer_Impl(const T &obj) : T(obj) { }

        constexpr inline SI s() const {
            return SI(T::low, T::high);
        }

        constexpr inline UI u() const {
            return UI(T::low, T::high);
        }

        constexpr inline static I ZERO() {
            return I(U::ZERO());
        };

        constexpr inline bool isZero() const {
            return T::low.isZero() && T::high.isZero();
        };

        constexpr inline bool upperBit() const {
            return T::high.upperBit();
        };

        constexpr inline static bool add_overflow(
                const I &v1, const I &v2, I &out) {
            return T::add_overflow(v1, v2, out);
        }

        constexpr inline static bool increment_overflow(I &value) {
            return T::increment_overflow(value);
        }

        constexpr inline static bool sub_overflow(
                const I &v1, const I &v2, I &out) {
            return T::sub_overflow(v1, v2, out);
        }

        constexpr inline static bool decrement_overflow(I &value) {
            return T::decrement_overflow(value);
        }

        constexpr inline static void leftShiftOneBit(I &value, bool bit) {
            U::leftShiftOneBit(value.high, value.low.upperBit());
            U::leftShiftOneBit(value.low, bit);
        }

        void printv(std::ostream &out) {
            T::high.printv(out);
            out << ", ";
            T::low.printv(out);
        }

        constexpr inline size_t numberOfLeadingZeros() const {
            if (T::high.isZero()) {
                return T::low.numberOfLeadingZeros() + half * 8;
            }
            return T::high.numberOfLeadingZeros();
        }

        constexpr inline size_t numberOfTrailingZeros() const {
            if (T::low.isZero()) {
                return T::high.numberOfTrailingZeros() + half * 8;
            }
            return T::low.numberOfTrailingZeros();
        }

        constexpr inline I operator+() const {
            return *this;
        }

        constexpr inline I operator-() const {
            return (~(*this)).p1();
        }

        template<typename II = Integer<half * 2, sig>>
        constexpr inline I operator/(const I &other) const {
            return divrem(II(*this), II(other)).value;
        }

        template<typename II = Integer<half * 2, sig>>
        constexpr inline I operator%(const I &other) const {
            II rem;
            divrem(II(*this), II(other), &rem);
            return rem.value;
        }

        constexpr inline I operator+(const I &other) const {
            U tmph = T::high + other.high;
            U tmpl = U::ZERO();
            if (U::add_overflow(T::low, other.low, tmpl)) {
                ++tmph;
            }
            return I(tmpl, tmph);
        }

        constexpr inline I operator-(const I &other) const {
            U tmph = T::high - other.high;
            U tmpl = U::ZERO();
            if (U::sub_overflow(T::low, other.low, tmpl)) {
                --tmph;
            }
            return I(tmpl, tmph);
        }

        constexpr inline I operator*(const I &other) const {
            auto tmp = wmultiply(T::low, other.low).value;
            return I(tmp.low, tmp.high + T::low * other.high + T::high * other.low);
        }

        constexpr inline I operator<<(const M other) const {
            return leftShift(*this, other & T::shmask);
        }

        constexpr inline I operator|(const I &other) const {
            return I(T::low | other.low, T::high | other.high);
        }

        constexpr inline I operator&(const I &other) const {
            return I(T::low & other.low, T::high & other.high);
        }

        constexpr inline I operator^(const I &other) const {
            return I(T::low ^ other.low, T::high ^ other.high);
        }

        constexpr inline I operator~() const {
            return I(~T::low, ~T::high);
        }

        constexpr inline bool operator==(const I &other) const {
            return (T::low == other.low) && (T::high == other.high);
        }

        constexpr inline bool operator!=(const I &other) const {
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
    constexpr inline bool p_compare_types_cv() {
        return p_compare_types<typename std::remove_cv<T1>::type,
                typename std::remove_cv<T2>::type>::value;
    }

    template<typename T>
    constexpr inline bool p_is_integral() {
        return std::is_integral<T>::value && (!p_compare_types_cv<T, bool>());
    }

    template<typename T>
    constexpr inline bool p_is_signed() {
        return std::is_signed<T>::value;
    }

    namespace p_i_seq {

        template <typename T, T... values>
        struct array_t {

            constexpr inline const T operator[](size_t index) const {
                static_assert(sizeof...(values) != 0, "zero-size array");
                constexpr T data[sizeof...(values)] = {values...};
                return data[index];
            }
        };

        template <typename T, size_t length>
        struct v_array_t {
            T data[max(size_t(1), length)];

            constexpr inline const T& operator[](size_t index) const {
                return data[index];
            }

            constexpr inline T& operator[](size_t index) {
                return data[index];
            }
        };

        template <typename T, size_t length>
        constexpr inline void unused_array(v_array_t<T, length>) { }

        template<typename T, T... v1, T... v2>
        constexpr inline array_t<T, v1..., v2...>
        append(array_t<T, v1...>, array_t<T, v2...>) {
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
        constexpr inline typename seq_h<T, f, l>::type make_array() {
            return {};
        }

        struct any {

            template<typename T>
            constexpr inline any(T) { }
        };

        template<size_t... i1>
        struct p_wrapper {
            template<size_t>
            using any_h = any;

            template<size_t... i2, typename T>
            constexpr inline static T
            get_value(any_h<i1>..., T value, any_h<i2>...) {
                return value;
            }
        };

        template<size_t... i1, size_t... i2, typename... Tp>
        constexpr inline auto p_element_h(array_t<size_t, i1...>,
                array_t<size_t, i2...>, Tp... arr) {
            return p_wrapper<i1...>::template get_value<i2...>(arr...);
        };

        template<size_t index, typename... Tp>
        constexpr inline auto element(Tp... arr) {
            return p_element_h(make_array<size_t, 0, index>(),
                    make_array<size_t, index + 1, sizeof...(arr)>(), arr...);
        };

        template<typename... Tp>
        constexpr inline auto last_element(Tp... arr) {
            return element<sizeof...(arr) - 1 > (arr...);
        };
    }

    template<int>
    struct p_Element_Types;

    template<>
    struct p_Element_Types<0> {
        typedef uint32_t U;
        typedef int32_t S;
        typedef uint64_t DU;
        typedef int64_t DS;
    };

    template<>
    struct p_Element_Types<1> {
        typedef uint8_t U;
        typedef int8_t S;
        typedef uint16_t DU;
        typedef int16_t DS;
    };

    template<>
    struct p_Element_Types<2> {
        typedef uint16_t U;
        typedef int16_t S;
        typedef uint32_t DU;
        typedef int32_t DS;
    };

    template<>
    struct p_Element_Types<3> {
        typedef uint8_t U;
        typedef int8_t S;
        typedef uint16_t DU;
        typedef int16_t DS;
    };

    template<size_t size, bool sig>
    class p_Array_Integer_Base;

    template<size_t size, bool sig>
    class p_Array_Integer_Impl;

    template<size_t size, bool sig>
    struct p_Integer_Impl <size, sig, p_IType::array> {
        typedef p_Array_Integer_Impl<size, sig> type;
    };

    template<size_t size>
    class p_Array_Integer_Base<size, false> {
    private:
        typedef p_Element_Types<size & 3> ET;
        typedef typename ET::S S;
        typedef typename ET::U U;
        typedef typename ET::DS DS;
        typedef typename ET::DU DU;
        typedef p_Array_Integer_Base I;
        typedef p_SHType<size> M;
        constexpr static M shdivider = size * 8;
        constexpr static size_t length = size / sizeof (U);
        template<typename Tp, Tp... values>
        using A = p_i_seq::array_t<Tp, values...>;
        template<size_t len>
        using AT = p_i_seq::v_array_t<U, len>;
        AT<length> data;

    public:

        constexpr inline p_Array_Integer_Base() = default;

    private:

        template<size_t a_len>
        constexpr inline static U value_for_index(
                AT<a_len> arr, size_t index, U fill) {
            return index < a_len ? arr[index] : fill;
        }

        template<size_t a_len, size_t... index>
        constexpr inline static AT<length> set_h(
                AT< a_len> arr, A<size_t, index...>, U fill) {
            return {value_for_index(arr, index, fill)...};
        }

    public:

        template<typename... Tp>
        constexpr explicit inline p_Array_Integer_Base(Tp... arr) :
        data(set_h(AT<sizeof...(Tp)>{U(arr)...},
        p_i_seq::make_array<size_t, 0, length>(),
                (p_i_seq::last_element(arr...) < 0) ? ~U(0) : U(0))) { }

        template<size_t size2, bool sig2>
        friend class p_Array_Integer_Impl;

        template<size_t size2, bool sig2>
        friend class Integer;
    };

    template<size_t size, bool sig>
    class p_Array_Integer_Impl : p_Array_Integer_Base<size, sig> {
    private:
        typedef p_Array_Integer_Base<size, sig> T;
        typedef p_Array_Integer_Impl<size, sig> I;
        typedef p_Array_Integer_Impl<size, false> UI;
        typedef p_Array_Integer_Impl<size, true> SI;
        typedef typename T::S S;
        typedef typename T::U U;
        typedef typename T::DS DS;
        typedef typename T::DU DU;
        typedef typename T::M M;
        template<typename Tp, Tp... values>
        using A = p_i_seq::array_t<Tp, values...>;
        template<size_t len>
        using AT = p_i_seq::v_array_t<U, len>;

    public:
        using T::T;

        constexpr inline p_Array_Integer_Impl(const T &obj) : T(obj) { }

        void printv(std::ostream &out) {
            out << std::hex;
            size_t i = T::length;
            out << uint32_t(T::data[--i]);
            do {
                out << ", ";
                out << uint32_t(T::data[--i]);
            } while (i > 0);
        }

    private:

        /*constexpr inline I p1() const {
            return p1_helper(T::low.p1(), T::high);
        }*/

    public:

        constexpr inline I operator+() const {
            return *this;
        }

        /*constexpr inline I operator-() const {
            return (~(*this)).p1();
        }*/

    private:

        template<size_t... index>
        constexpr inline static bool isZero_h(const I &v, A<size_t, index...>) {
            bool out = true;
            p_i_seq::unused_array<bool, sizeof...(index)>({(out &= (v.data[index] == 0))...});
            return out;
        }

    public:

        constexpr inline bool isZero() const {
            return isZero_h(*this, p_i_seq::make_array<size_t, 0, T::length>());
        };

    private:

        template<size_t... index>
        constexpr inline static I neg_h(const I &v, A<size_t, index...>) {
            return I((~v.data[index])...);
        }

    public:

        constexpr inline I operator~() const {
            return neg_h(*this, p_i_seq::make_array<size_t, 0, T::length>());
        }

    private:

        template<size_t... index>
        constexpr inline static I or_h(const I &v1, const I &v2, A<size_t, index...>) {
            return I((v1.data[index] | v2.data[index])...);
        }

        template<size_t... index>
        constexpr inline static I and_h(const I &v1, const I &v2, A<size_t, index...>) {
            return I((v1.data[index] & v2.data[index])...);
        }

        template<size_t... index>
        constexpr inline static I xor_h(const I &v1, const I &v2, A<size_t, index...>) {
            return I((v1.data[index] ^ v2.data[index])...);
        }

    public:

        constexpr inline I operator|(const I &v2) const {
            return or_h(*this, v2, p_i_seq::make_array<size_t, 0, T::length>());
        }

        constexpr inline I operator&(const I &v2) const {
            return and_h(*this, v2, p_i_seq::make_array<size_t, 0, T::length>());
        }

        constexpr inline I operator^(const I &v2) const {
            return xor_h(*this, v2, p_i_seq::make_array<size_t, 0, T::length>());
        }

        template<size_t size2, bool sig2>
        friend class Integer;
    };

    template<typename U, typename S, bool sig, typename V, p_enable_if(sig)>
    constexpr inline S p_castUS(V value) {
        return S(value);
    }

    template<typename U, typename S, bool sig, typename V, p_enable_if(!sig)>
    constexpr inline U p_castUS(V value) {
        return U(value);
    }

    template<typename T, bool sig, p_enable_if(sig)>
    constexpr inline T p_min_value() {
        return T(1) << (sizeof (T) * 8 - 1);
    }

    template<typename T, bool sig, p_enable_if(!sig)>
    constexpr inline T p_min_value() {
        return T::ZERO();
    }

    template<typename T, bool sig>
    constexpr inline T p_max_value() {
        return ~p_min_value<T, sig>();
    }

    template<size_t size, typename UI = Integer<size, false>,
    p_enable_if(p_getIntegerType(size) == pow2)>
    constexpr inline UI p_udivrem(const Integer<size, false> &a,
            const Integer<size, false> &b, Integer<size, false> *rem) {
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
            const Integer<size, true> &y, Integer<size, true> *rem) {
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
                const I &x, const I &y, I *rem) {
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
                const UI &x, const UI &y, UI *rem) {
            return p_udivrem(x, y, rem);
        }
    };

    template<size_t size>
    struct p_divrem_h<size, true, pow2> {

        template<typename SI = Integer<size, true >>
        constexpr inline static SI divrem(
                const SI &x, const SI &y, SI *rem) {
            return p_sdivrem(x, y, rem);
        }
    };

    template<size_t size, bool sig>
    constexpr inline Integer<size, sig> divrem(const Integer<size, sig> &x,
            const Integer<size, sig> &y) {
        return p_divrem_h<size, sig>::divrem(x, y, (Integer<size, sig>*)nullptr);
    }

    template<size_t size, bool sig>
    constexpr inline Integer<size, sig> divrem(const Integer<size, sig> &x,
            const Integer<size, sig> &y, Integer<size, sig> *rem) {
        return p_divrem_h<size, sig>::divrem(x, y, rem);
    }

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

            constexpr inline static Integer<size2, sig2> upcast(const Integer &v) {
                using I = Integer<size2, sig2>;
                return typename I::V(p_castUS<typename V::U,
                        typename V::S, sig > (v.value.value));
            }
        };

        template<size_t size2, bool sig2>
        struct upcast_h<size2, sig2, false, true, false> {

            constexpr inline static Integer<size2, sig2> upcast(const Integer &v) {
                using I = Integer<size2, sig2>;
                return I(typename I::V(v.value.low, v.value.high));
            }
        };

        template<size_t size2, bool sig2>
        struct upcast_h<size2, sig2, false, false, true> {

            constexpr inline static Integer<size2, sig2> upcast(const Integer &v) {
                using I = Integer<size2, sig2>;
                return typename I::V(p_castUS<typename I::V::U,
                        typename I::V::S, sig > (v));
            }
        };

        template<size_t size2, bool sig2>
        constexpr inline Integer<size2, sig2> upcast() const {
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

            constexpr inline static Integer<size2, sig2> downcast(const Integer &v) {
                using I = Integer<size2, sig2>;
                return typename I::V(typename I::V::U(v.value.value));
            }
        };

        template<size_t size2, bool sig2>
        struct downcast_h<size2, sig2, false, true> {

            constexpr inline static Integer<size2, sig2> downcast(const Integer &v) {
                using I = Integer<size2, sig2>;
                return I(v.value.low);
            }
        };

        template<size_t size2, bool sig2>
        constexpr inline Integer<size2, sig2> downcast() const {
            return downcast_h<size2, sig2>::downcast(*this);
        }

        constexpr inline Integer p1() const {
            return value.p1();
        }

        constexpr inline Integer m1() const {
            return value.m1();
        }

        constexpr inline Integer(const V n) : value(n) { }
    public:

        constexpr inline static size_t SIZE() {
            return size;
        };

        constexpr inline static bool is_signed() {
            return sig;
        };

        constexpr inline static Integer ZERO() {
            return V::ZERO();
        };

        constexpr inline static Integer ONE() {
            return ZERO().p1();
        };

        constexpr inline static Integer MAX_VALUE() {
            return p_max_value<Integer, sig>();
        };

        constexpr inline static Integer MIN_VALUE() {
            return p_min_value<Integer, sig>();
        };

        constexpr inline Integer() = default;

        constexpr inline bool isZero() const {
            return value.isZero();
        };

        constexpr inline bool isNegative() const {
            return value.isNegative();
        };

        constexpr inline bool upperBit() const {
            return value.upperBit();
        };

        constexpr inline static bool add_overflow(
                const Integer &v1, const Integer &v2, Integer &out) {
            return V::add_overflow(v1.value, v2.value, out.value);
        }

        constexpr inline static bool increment_overflow(Integer &value) {
            return V::increment_overflow(value.value);
        }

        constexpr inline static bool sub_overflow(
                const Integer &v1, const Integer &v2, Integer &out) {
            return V::sub_overflow(v1.value, v2.value, out.value);
        }

        constexpr inline static bool decrement_overflow(Integer &value) {
            return V::decrement_overflow(value.value);
        }

        constexpr inline static void leftShiftOneBit(Integer &value, bool bit) {
            V::leftShiftOneBit(value.value, bit);
        }

        constexpr inline size_t numberOfLeadingZeros() const {
            return value.numberOfLeadingZeros();
        }

        constexpr inline size_t numberOfTrailingZeros() const {
            return value.numberOfTrailingZeros();
        }

        constexpr inline Integer<size, true> s() const {
            return *this;
        }

        constexpr inline Integer<size, false> u() const {
            return *this;
        }

        constexpr inline const typename V::U& ulow() const {
            return value.low;
        }

        constexpr inline typename V::U& ulow() {
            return value.low;
        }

        constexpr inline const typename V::U& uhigh() const {
            return value.high;
        }

        constexpr inline typename V::U& uhigh() {
            return value.high;
        }

        constexpr inline typename V::U& uvalue() {
            return value.value;
        }

        constexpr inline const typename V::U& uvalue() const {
            return value.value;
        }

        void print(std::ostream &out) {
            out << "I<" << std::dec << size << ", " << (sig ? "t" : "f") << ">{";
            value.printv(out);
            out << "}";
        }

        void printv(std::ostream &out) {
            value.printv(out);
        }

        template<size_t size1, bool sig1, size_t size2, bool sig2,
        p_enable_if((p_getIntegerType(size) == pow2) &&
                (size1 <= size / 2) && (size2 <= size / 2))>
        constexpr inline Integer(const Integer<size1, sig1> &low,
                const Integer<size2, sig2> &high) : value(low, high) { }

        template<typename T1, typename T2,
        p_enable_if((p_getIntegerType(size) == pow2) &&
                (p_is_integral<T1>()) && (p_is_integral<T2>())&&
                (sizeof (T1) <= size / 2) && (sizeof (T2) <= size / 2))>
        constexpr inline Integer(const T1 low, const T2 high) : value(low, high) { }

        template<typename T1, size_t size2, bool sig2,
        p_enable_if((p_getIntegerType(size) == pow2) &&
                (sizeof (T1) <= size / 2) && (size2 <= size / 2))>
        constexpr inline Integer(const T1 low,
                const Integer<size2, sig2> &high) : value(low, high) { }

        template<size_t size1, bool sig1, typename T2,
        p_enable_if((p_getIntegerType(size) == pow2) &&
                (size1 <= size / 2) && (sizeof (T2) <= size / 2))>
        constexpr inline Integer(const Integer<size1, sig1> &low,
                const T2 high) : value(low, high) { }

    private:

        template<typename T, p_IType = p_getIntegerType(size)>
        struct tcast_h {
        };

        template<typename T>
        struct tcast_h<T, native> {

            constexpr inline static V tcast(const T n) {
                return V(n);
            }
        };

        template<typename T>
        struct tcast_h<T, pow2> {

            constexpr inline static V tcast(const T n) {
                return Integer(Integer<sizeof (T), JIO::p_is_signed<T>()>(n)).value;
            }
        };

    public:

        template<typename T, p_enable_if((p_is_integral<T>()) && (size >= sizeof (T)))>
        constexpr inline Integer(const T n) : value(tcast_h<T>::tcast(n)) { }

        template<typename T, p_enable_if((p_is_integral<T>()) && (size < sizeof (T)))>
        constexpr explicit inline Integer(const T n) : value(tcast_h<T>::tcast(n)) { }

        template<size_t size2, bool sig2,
        p_enable_if(size2 >= size)>
        constexpr inline operator Integer<size2, sig2>() const {
            return upcast<size2, sig2>();
        }

        template<size_t size2, bool sig2,
        p_enable_if(size2 < size)>
        constexpr explicit inline operator Integer<size2, sig2>() const {
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

            constexpr inline static T pcast(const Integer& v) {
                return T(p_castUS<typename V::U, typename V::S,
                        sig, typename V::U > (v.value.value));
            }
        };

        template<typename T>
        struct pcast_h<T, false, true, false> {

            constexpr inline static T pcast(const Integer& v) {
                return T(v.value.value);
            }
        };

        template<typename T>
        struct pcast_h<T, false, false, true> {

            constexpr inline static T pcast(const Integer& v) {
                return T(Integer<sizeof (T), JIO::p_is_signed<T>()>(v));
            }
        };

    public:

        template<typename T, p_enable_if((p_is_integral<T>()) && (sizeof (T) >= size))>
        constexpr inline operator T() const {
            return pcast_h<T>::pcast(*this);
        }

        template<typename T, p_enable_if((p_is_integral<T>()) && (sizeof (T) < size))>
        constexpr explicit inline operator T() const {
            return pcast_h<T>::pcast(*this);
        }

        constexpr explicit inline operator bool() const {
            return !(this->isZero());
        }

        template<size_t size2, bool sig2,
        typename R = Integer<max(size, size2), sig && sig2>>
        constexpr inline R operator+(const Integer<size2, sig2> &v2) const {
            return R(*this).value + R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = Integer<max(size, size2), sig && sig2>>
        constexpr inline R operator-(const Integer<size2, sig2> &v2) const {
            return R(*this).value - R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = Integer<max(size, size2), sig && sig2>>
        constexpr inline R operator*(const Integer<size2, sig2> &v2) const {
            return R(*this).value * R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = Integer<max(size, size2), sig && sig2>>
        constexpr inline R operator/(const Integer<size2, sig2> &v2) const {
            return typename R::V(R(*this).value / R(v2).value);
        }

        template<size_t size2, bool sig2,
        typename R = Integer<max(size, size2), sig && sig2>>
        constexpr inline R operator%(const Integer<size2, sig2> &v2) const {
            return typename R::V(R(*this).value % R(v2).value);
        }

        template<size_t size2, bool sig2,
        typename R = Integer<max(size, size2), sig && sig2>>
        constexpr inline R operator|(const Integer<size2, sig2> &v2) const {
            return R(*this).value | R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = Integer<max(size, size2), sig && sig2>>
        constexpr inline R operator&(const Integer<size2, sig2> &v2) const {
            return R(*this).value & R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = Integer<max(size, size2), sig && sig2>>
        constexpr inline R operator^(const Integer<size2, sig2> &v2) const {
            return R(*this).value ^ R(v2).value;
        }

        template<size_t size2, bool sig2>
        constexpr inline Integer operator<<(const Integer<size2, sig2> &v2) const {
            return value << p_SHType<size>(v2);
        }

        template<typename T, p_enable_if((p_is_integral<T>()))>
        constexpr inline Integer operator<<(const T v2) const {
            return value << p_SHType<size>(v2);
        }

        template<size_t size2, bool sig2>
        constexpr inline Integer operator>>(const Integer<size2, sig2> &v2) const {
            return V(value >> p_SHType<size>(v2));
        }

        template<typename T, p_enable_if((p_is_integral<T>()))>
        constexpr inline Integer operator>>(const T v2) const {
            return V(value >> p_SHType<size>(v2));
        }

        template<size_t size1, bool sig1, size_t size2, bool sig2>
        friend constexpr bool operator==(
                const Integer<size1, sig1> &v1,
                const Integer<size2, sig2> &v2);

        template<size_t size1, bool sig1, size_t size2, bool sig2>
        friend constexpr bool operator!=(
                const Integer<size1, sig1> &v1,
                const Integer<size2, sig2> &v2);

        template<size_t size1, bool sig1, size_t size2, bool sig2>
        friend constexpr bool operator<=(
                const Integer<size1, sig1> &v1,
                const Integer<size2, sig2> &v2);

        template<size_t size1, bool sig1, size_t size2, bool sig2>
        friend constexpr bool operator>=(
                const Integer<size1, sig1> &v1,
                const Integer<size2, sig2> &v2);

        template<size_t size1, bool sig1, size_t size2, bool sig2>
        friend constexpr bool operator<(
                const Integer<size1, sig1> &v1,
                const Integer<size2, sig2> &v2);

        template<size_t size1, bool sig1, size_t size2, bool sig2>
        friend constexpr bool operator>(
                const Integer<size1, sig1> &v1,
                const Integer<size2, sig2> &v2);

        constexpr inline Integer operator~() const {
            return ~value;
        }

        constexpr inline Integer operator-() const {
            return -value;
        }

        constexpr inline Integer operator+() const {
            return *this;
        }

        template<size_t size1, bool sig1>
        friend constexpr Integer<size1, sig1>&
        operator++(Integer<size1, sig1>&);

        template<size_t size1, bool sig1>
        friend constexpr Integer<size1, sig1>&
        operator--(Integer<size1, sig1>&);

        template<size_t size2, bool sig2>
        friend class Integer;

        template<size_t size2, bool sig2>
        friend class p_Pow2_Integer_Impl;

        template<size_t size2, bool sig2>
        friend class p_Array_Integer_Impl;

        template<typename T>
        friend struct p_Operators_Impl;
    };

    template <typename T>
    constexpr inline size_t logb2(const T value) {
        return sizeof (T) * 8 - p_numberOfLeadingZeros_h(value);
    }

    template <size_t size, bool sig>
    constexpr inline size_t logb2(const Integer<size, sig> &value) {
        return size * 8 - value.numberOfLeadingZeros();
    }

    template<size_t size1, bool sig1>
    std::ostream& operator<<(std::ostream &out, Integer<size1, sig1> v) {
        auto f = out.flags();
        v.print(out);
        out.flags(f);
        return out;
    }

    template<size_t size1, bool sig1>
    constexpr inline Integer<size1, sig1>& operator++(Integer<size1, sig1> &v1) {
        return v1 = v1.p1();
    }

    template<size_t size1, bool sig1>
    constexpr inline Integer<size1, sig1>& operator--(Integer<size1, sig1> &v1) {
        return v1 = v1.m1();
    }

    template<size_t size1, bool sig1>
    constexpr inline Integer<size1, sig1> operator++(Integer<size1, sig1> &v1, int) {
        Integer<size1, sig1> tmp = v1;
        ++v1;
        return tmp;
    }

    template<size_t size1, bool sig1>
    constexpr inline Integer<size1, sig1> operator--(Integer<size1, sig1> &v1, int) {
        Integer<size1, sig1> tmp = v1;
        --v1;
        return tmp;
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator+(const Integer<size1, sig1> &v1, const T v2) {
        return R(v1) + R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator+(const T v1, const Integer<size1, sig1> &v2) {
        return R(v1) + R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>& operator+=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 + Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (sizeof (T) <= size1))>
    constexpr inline Integer<size1, sig1>& operator+=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 + Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (size1 <= sizeof (T)))>
    constexpr inline T& operator+=(T &v1, const Integer<size1, sig1> &v2) {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) + R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator-(const Integer<size1, sig1> &v1, const T v2) {
        return R(v1) - R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator-(const T v1, const Integer<size1, sig1> &v2) {
        return R(v1) - R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>& operator-=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 - Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (sizeof (T) <= size1))>
    constexpr inline Integer<size1, sig1>& operator-=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 - Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (size1 <= sizeof (T)))>
    constexpr inline T& operator-=(T &v1, const Integer<size1, sig1> &v2) {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) - R(v2));
    }

    template<size_t size1, p_enable_if((size1 == 1) || (size1 == 2) || (size1 == 4))>
    constexpr inline Integer<size1 * 2, false> wmultiply(
            const Integer<size1, false> &v1, const Integer<size1, false> &v2) {
        using R = Integer<size1 * 2, false>;
        return R(v1) * R(v2);
    }

    template<size_t size1, p_enable_if((size1 == 8) || p_getIntegerType(size1) == pow2)>
    constexpr inline Integer<size1 * 2, false> wmultiply(
            const Integer<size1, false> &v1,
            const Integer<size1, false> &v2) {
        using U1 = Integer < size1 / 2, false >;
        using U2 = Integer<size1, false>;
        using U4 = Integer<size1 * 2, false>;
        U1 a = U1(v1 >> (size1 * 4)), b = U1(v1);
        U1 c = U1(v2 >> (size1 * 4)), d = U1(v2);
        U2 ac = wmultiply(a, c);
        U2 bd = wmultiply(b, d);
#if 1 //The Karatsuba algorithm
        U1 ab = U1::ZERO(), cd = U1::ZERO();
        bool abo = U1::add_overflow(a, b, ab);
        bool cdo = U1::add_overflow(c, d, cd);
        U2 abcd_low = wmultiply(ab, cd);
        unsigned int abcd_high = abo && cdo;
        if (abo && U2::add_overflow(U2(abcd_low), U2(cd) << (size1 * 4), abcd_low)) {
            ++abcd_high;
        }
        if (cdo && U2::add_overflow(U2(abcd_low), U2(ab) << (size1 * 4), abcd_low)) {
            ++abcd_high;
        }
        if (U2::sub_overflow(U2(abcd_low), ac, abcd_low)) {
            --abcd_high;
        }
        if (U2::sub_overflow(U2(abcd_low), bd, abcd_low)) {
            --abcd_high;
        }
        return U4(bd, ac) + (U4(abcd_low, abcd_high) << (size1 * 4));
#else
        U2 ad = wmultiply(a, d);
        U2 bc = wmultiply(b, c);
        return U4(bd, ac) + ((U4(ad) + U4(bc)) << (size1 * 4));
#endif
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator*(const Integer<size1, sig1> &v1, const T v2) {
        return R(v1) * R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator*(const T v1, const Integer<size1, sig1> &v2) {
        return R(v1) * R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>& operator*=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 * Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (sizeof (T) <= size1))>
    constexpr inline Integer<size1, sig1>& operator*=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 * Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (size1 <= sizeof (T)))>
    constexpr inline T& operator*=(T &v1, const Integer<size1, sig1> &v2) {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) * R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator/(const Integer<size1, sig1> &v1, const T v2) {
        return R(v1) / R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator/(const T v1, const Integer<size1, sig1> &v2) {
        return R(v1) / R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>& operator/=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 / Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (sizeof (T) <= size1))>
    constexpr inline Integer<size1, sig1>& operator/=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 / Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (size1 <= sizeof (T)))>
    constexpr inline T& operator/=(T &v1, const Integer<size1, sig1> &v2) {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) / R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator%(const Integer<size1, sig1> &v1, const T v2) {
        return R(v1) % R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator%(const T v1, const Integer<size1, sig1> &v2) {
        return R(v1) % R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>& operator%=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 % Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (sizeof (T) <= size1))>
    constexpr inline Integer<size1, sig1>& operator%=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 % Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (size1 <= sizeof (T)))>
    constexpr inline T& operator%=(T &v1, const Integer<size1, sig1> &v2) {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) % R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator|(const Integer<size1, sig1> &v1, const T v2) {
        return R(v1) | R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator|(const T v1, const Integer<size1, sig1> &v2) {
        return R(v1) | R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>& operator|=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 | Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (sizeof (T) <= size1))>
    constexpr inline Integer<size1, sig1>& operator|=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 | Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (size1 <= sizeof (T)))>
    constexpr inline T& operator|=(T &v1, const Integer<size1, sig1> &v2) {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) | R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator&(const Integer<size1, sig1> &v1, const T v2) {
        return R(v1) & R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator&(const T v1, const Integer<size1, sig1> &v2) {
        return R(v1) & R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>& operator&=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 & Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (sizeof (T) <= size1))>
    constexpr inline Integer<size1, sig1>& operator&=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 & Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (size1 <= sizeof (T)))>
    constexpr inline T& operator&=(T &v1, const Integer<size1, sig1> &v2) {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) & R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator^(const Integer<size1, sig1> &v1, const T v2) {
        return R(v1) ^ R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator^(const T v1, const Integer<size1, sig1> &v2) {
        return R(v1) ^ R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>& operator^=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 ^ Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (sizeof (T) <= size1))>
    constexpr inline Integer<size1, sig1>& operator^=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 ^ Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (size1 <= sizeof (T)))>
    constexpr inline T& operator^=(T &v1, const Integer<size1, sig1> &v2) {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) ^ R(v2));
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    constexpr inline bool operator==(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        using R = Integer < max(size1, size2), sig1 && sig2>;
        return R(v1).value == R(v2).value;
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator==(const Integer<size1, sig1> &v1, const T v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) == R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator==(const T v1, const Integer<size1, sig1> &v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) == R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    constexpr inline bool operator!=(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        using R = Integer < max(size1, size2), sig1 && sig2>;
        return R(v1).value != R(v2).value;
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator!=(const Integer<size1, sig1> &v1, const T v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) != R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator!=(const T v1, const Integer<size1, sig1> &v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) != R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    constexpr inline bool operator<=(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        using R = Integer < max(size1, size2), sig1 && sig2>;
        return R(v1).value <= R(v2).value;
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator<=(const Integer<size1, sig1> &v1, const T v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) <= R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator<=(const T v1, const Integer<size1, sig1> &v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) <= R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    constexpr inline bool operator>=(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        using R = Integer < max(size1, size2), sig1 && sig2>;
        return R(v1).value >= R(v2).value;
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator>=(const Integer<size1, sig1> &v1, const T v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) >= R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator>=(const T v1, const Integer<size1, sig1> &v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) >= R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    constexpr inline bool operator<(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        using R = Integer < max(size1, size2), sig1 && sig2>;
        return R(v1).value < R(v2).value;
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator<(const Integer<size1, sig1> &v1, const T v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) < R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator<(const T v1, const Integer<size1, sig1> &v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) < R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    constexpr inline bool operator>(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        using R = Integer < max(size1, size2), sig1 && sig2>;
        return R(v1).value > R(v2).value;
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator>(const Integer<size1, sig1> &v1, const T v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) > R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator>(const T v1, const Integer<size1, sig1> &v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) > R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline T operator<<(const T v1, const Integer<size1, sig1> &v2) {
        using R = Integer <sizeof (T), (p_is_signed<T>())>;
        return T(R(v1) << v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    constexpr inline Integer<size1, sig1>& operator<<=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 << v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline Integer<size1, sig1>& operator<<=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 << v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline T& operator<<=(T &v1, const Integer<size1, sig1> &v2) {
        return v1 = T(Integer<sizeof (T), p_is_signed<T>()>(v1) << v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline T operator>>(const T v1, const Integer<size1, sig1> &v2) {
        using R = Integer <sizeof (T), (p_is_signed<T>())>;
        return T(R(v1) >> v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    constexpr inline Integer<size1, sig1>& operator>>=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 >> v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline Integer<size1, sig1>& operator>>=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 >> v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline T& operator>>=(T &v1, const Integer<size1, sig1> &v2) {
        return v1 = T(Integer<sizeof (T), p_is_signed<T>()>(v1) >> v2);
    }

#undef p_enable_if
}

#endif /* INTEGER_HPP */

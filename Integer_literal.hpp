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

#ifndef INTEGER_LITERAL_HPP
#define INTEGER_LITERAL_HPP

#include "Integer.hpp"

// -std=c++14
namespace JIO {
    template<bool A>
    using p_enable_if_t = typename std::enable_if<A, bool>::type;

#define p_enable_if(B) p_enable_if_t<(B)> = false

    template <typename T, T... values>
    struct p_array_t {
        const T data[max(size_t(1), sizeof...(values))] = {values...};

        constexpr inline const T operator[](size_t index) const {
            return data[index];
        }
    };

    template<typename T, T value, T... values>
    constexpr inline auto p_append(p_array_t<T, values...>) noexcept {
        return p_array_t<T, value, values...>();
    }

    constexpr char p_digits[22] = {
        '0', '1', '2', '3', '4', '5',
        '6', '7', '8', '9', 'a', 'b',
        'c', 'd', 'e', 'f', 'A', 'B',
        'C', 'D', 'E', 'F'
    };

    template<char c, size_t index = 0, p_enable_if(p_digits[index] == c)>
    constexpr inline auto p_indexOfDigit() noexcept {
        return index < 16 ? index : index - 6;
    }

    template<char c, size_t index = 0, p_enable_if(p_digits[index] != c)>
    constexpr inline auto p_indexOfDigit() noexcept {
        return p_indexOfDigit<c, index + 1 > ();
    }

    constexpr inline size_t p_make_pow2(size_t i) noexcept {
        if (i == 0) {
            return 0;
        }
        size_t log = logb2(i);
        if (i == (size_t(1) << (log - 1))) {
            return i;
        }
        return 1 << log;
    }

    constexpr size_t p_bits_to_bytes(size_t bits, bool sig) noexcept {
        return p_make_pow2((bits + 7 + (sig ? 1 : 0)) / 8);
    }

    template<bool sig, int bit_per_symbol, int first, int... nums>
    constexpr inline auto p_get_bytes() noexcept {
        return p_bits_to_bytes(sizeof...(nums) * bit_per_symbol +
                (4 - p_numberOfLeadingZeros4Bit_h(uint8_t(first))), sig);
    }

    template<bool sig, int... nums, size_t bytes = p_get_bytes<sig, 4, nums...>()>
    constexpr inline auto p_parseHex(p_array_t<int, nums...> arr) noexcept {
        using I = Integer<bytes, sig>;
        I out = I::ZERO();
        for (size_t i = 0; i<sizeof...(nums); i++) {
            out <<= 4;
            out |= I(arr[i]);
        }
        return out;
    }

    template<bool sig, size_t bytes, int... nums>
    constexpr inline auto p_parseDec_h(p_array_t<int, nums...> arr) noexcept {
        using I = Integer<bytes, sig>;
        I out = I::ZERO();
        for (size_t i = 0; i<sizeof...(nums); i++) {
            out *= I(10);
            out += I(arr[i]);
        }
        return out;
    }

    template<bool sig, int... nums, size_t tmp_bytes =
    p_bits_to_bytes(sizeof...(nums) * 4, false),
    size_t bytes = p_bits_to_bytes(
            tmp_bytes * 8 - p_parseDec_h<false, tmp_bytes>(
            p_array_t<int, nums...>()).numberOfLeadingZeros(), sig)>
    constexpr inline auto p_parseDec(p_array_t<int, nums...> arr) noexcept {
        return p_parseDec_h<sig, bytes>(arr);
    }

    template<bool sig, int... nums, size_t bytes = p_get_bytes<sig, 3, nums...>()>
    constexpr inline auto p_parseOct(p_array_t<int, nums...> arr) noexcept {
        using I = Integer<bytes, sig>;
        I out = I::ZERO();
        for (size_t i = 0; i<sizeof...(nums); i++) {
            out <<= 3;
            out |= I(arr[i]);
        }
        return out;
    }

    template<bool sig, int... nums, size_t bytes = p_get_bytes<sig, 1, nums...>()>
    constexpr inline auto p_parseBin(p_array_t<int, nums...> arr) noexcept {
        using I = Integer<bytes, sig>;
        I out = I::ZERO();
        for (size_t i = 0; i<sizeof...(nums); i++) {
            out <<= 1;
            out |= I(arr[i]);
        }
        return out;
    }

    template<int min, int max, int value>
    constexpr inline bool p_contains() noexcept {
        return value >= min && value <= max;
    }

    template<int min, int max>
    constexpr inline auto p_verify() noexcept {
        return p_array_t<int>();
    }

    template<int min, int max, char a, char... chars,
    p_enable_if((p_contains<min, max, p_indexOfDigit<a>()>()))>
    constexpr inline auto p_verify() noexcept {
        return p_append<int, p_indexOfDigit<a>()> (p_verify<min, max, chars...>());
    }

    template<int min, int max, char a, char... chars, p_enable_if(a != '0')>
    constexpr inline auto p_filter() noexcept {
        return p_verify<min, max, a, chars...>();
    }

    template<int min, int max, char a, char... chars, p_enable_if(a == '0')>
    constexpr inline auto p_filter() noexcept {
        return p_filter<min, max, chars...>();
    }

    template<bool sig, char a, char b, char... chars, p_enable_if((a == '0') &&
            !(b == 'b' || b == 'B' || b == 'x' || b == 'X'))>
    constexpr inline auto p_parse() noexcept {
        return p_parseOct<sig>(p_filter<0, 7, b, chars...>());
    }

    template<bool sig, char a, char b, char... chars, p_enable_if((a == '0') &&
            (b == 'b' || b == 'B'))>
    constexpr inline auto p_parse() noexcept {
        return p_parseBin<sig>(p_filter<0, 1, chars...>());
    }

    template<bool sig, char a, char b, char... chars, p_enable_if((a == '0') &&
            (b == 'x' || b == 'X'))>
    constexpr inline auto p_parse() noexcept {
        return p_parseHex<sig>(p_filter<0, 15, chars...>());
    }

    template<bool sig, char a, char... chars, p_enable_if(a != '0')>
    constexpr inline auto p_parse() noexcept {
        return p_parseDec<sig>(p_filter<0, 10, a, chars...>());
    }

    template<bool sig, char a, p_enable_if(a == '0')>
    constexpr inline auto p_parse() noexcept {
        return Integer<1, sig>(0);
    }

    template<char... chars>
    constexpr inline auto operator""_UI() noexcept {
        return p_parse<false, chars...>();
    }

    template<char... chars>
    constexpr inline auto operator""_SI() noexcept {
        return p_parse<true, chars...>();
    }

#undef p_enable_if
}

#endif /* INTEGER_LITERAL_HPP */


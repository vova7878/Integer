#include <gtest/gtest.h>
#include <Integer.hpp>
#include <type_traits>

using namespace JIO;

template<size_t size>
using U = Integer<size, false>;
template<size_t size>
using S = Integer<size, true>;

#define ASSERT_EQ_TYPE(T1, T2) \
ASSERT_TRUE((std::is_same<T1, T2>()))
#define EXPECT_EQ_TYPE(T1, T2) \
EXPECT_TRUE((std::is_same<T1, T2>()))

TEST(Literals, unsigned_0_return_type) {
    EXPECT_EQ_TYPE(U<1>, decltype(0_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(0x0_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(00_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(0b0_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(0x00000000_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(000000000_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(0b00000000_UI));
}

TEST(Literals, unsigned_1_return_type) {
    EXPECT_EQ_TYPE(U<1>, decltype(0x1_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(0x7f_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(0x80_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(0xff_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(1_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(127_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(128_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(255_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(01_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(0177_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(0200_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(0377_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(0b1_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(0b1111111_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(0b10000000_UI));
    EXPECT_EQ_TYPE(U<1>, decltype(0b11111111_UI));
}

TEST(Literals, unsigned_2_return_type) {
    EXPECT_EQ_TYPE(U<2>, decltype(0x100_UI));
    EXPECT_EQ_TYPE(U<2>, decltype(0x7fff_UI));
    EXPECT_EQ_TYPE(U<2>, decltype(0x8000_UI));
    EXPECT_EQ_TYPE(U<2>, decltype(0xffff_UI));
    EXPECT_EQ_TYPE(U<2>, decltype(256_UI));
    EXPECT_EQ_TYPE(U<2>, decltype(32767_UI));
    EXPECT_EQ_TYPE(U<2>, decltype(32768_UI));
    EXPECT_EQ_TYPE(U<2>, decltype(65535_UI));
    EXPECT_EQ_TYPE(U<2>, decltype(0400_UI));
    EXPECT_EQ_TYPE(U<2>, decltype(077777_UI));
    EXPECT_EQ_TYPE(U<2>, decltype(0100000_UI));
    EXPECT_EQ_TYPE(U<2>, decltype(0177777_UI));
    EXPECT_EQ_TYPE(U<2>, decltype(0b100000000_UI));
    EXPECT_EQ_TYPE(U<2>, decltype(0b111111111111111_UI));
    EXPECT_EQ_TYPE(U<2>, decltype(0b1000000000000000_UI));
    EXPECT_EQ_TYPE(U<2>, decltype(0b1111111111111111_UI));
}

TEST(Literals, unsigned_4_return_type) {
    EXPECT_EQ_TYPE(U<4>, decltype(0x10000_UI));
    EXPECT_EQ_TYPE(U<4>, decltype(0x7fffffff_UI));
    EXPECT_EQ_TYPE(U<4>, decltype(0x80000000_UI));
    EXPECT_EQ_TYPE(U<4>, decltype(0xffffffff_UI));
    EXPECT_EQ_TYPE(U<4>, decltype(65536_UI));
    EXPECT_EQ_TYPE(U<4>, decltype(2147483647_UI));
    EXPECT_EQ_TYPE(U<4>, decltype(2147483648_UI));
    EXPECT_EQ_TYPE(U<4>, decltype(4294967295_UI));
    EXPECT_EQ_TYPE(U<4>, decltype(0200000_UI));
    EXPECT_EQ_TYPE(U<4>, decltype(017777777777_UI));
    EXPECT_EQ_TYPE(U<4>, decltype(020000000000_UI));
    EXPECT_EQ_TYPE(U<4>, decltype(037777777777_UI));
    EXPECT_EQ_TYPE(U<4>, decltype(0b10000000000000000_UI));
    EXPECT_EQ_TYPE(U<4>, decltype(0b1111111111111111111111111111111_UI));
    EXPECT_EQ_TYPE(U<4>, decltype(0b10000000000000000000000000000000_UI));
    EXPECT_EQ_TYPE(U<4>, decltype(0b11111111111111111111111111111111_UI));
}

TEST(Literals, unsigned_8_return_type) {
    EXPECT_EQ_TYPE(U<8>, decltype(0x100000000_UI));
    EXPECT_EQ_TYPE(U<8>, decltype(0x7fffffffffffffff_UI));
    EXPECT_EQ_TYPE(U<8>, decltype(0x8000000000000000_UI));
    EXPECT_EQ_TYPE(U<8>, decltype(0xffffffffffffffff_UI));
    EXPECT_EQ_TYPE(U<8>, decltype(4294967296_UI));
    EXPECT_EQ_TYPE(U<8>, decltype(9223372036854775807_UI));
    EXPECT_EQ_TYPE(U<8>, decltype(9223372036854775808_UI));
    EXPECT_EQ_TYPE(U<8>, decltype(18446744073709551615_UI));
    EXPECT_EQ_TYPE(U<8>, decltype(040000000000_UI));
    EXPECT_EQ_TYPE(U<8>, decltype(0777777777777777777777_UI));
    EXPECT_EQ_TYPE(U<8>, decltype(01000000000000000000000_UI));
    EXPECT_EQ_TYPE(U<8>, decltype(01777777777777777777777_UI));
    EXPECT_EQ_TYPE(U<8>, decltype(0b100000000000000000000000000000000_UI));
    EXPECT_EQ_TYPE(U<8>, decltype(0b111111111111111111111111111111111111111111111111111111111111111_UI));
    EXPECT_EQ_TYPE(U<8>, decltype(0b1000000000000000000000000000000000000000000000000000000000000000_UI));
    EXPECT_EQ_TYPE(U<8>, decltype(0b1111111111111111111111111111111111111111111111111111111111111111_UI));
}

TEST(Literals, unsigned_16_return_type) {
    EXPECT_EQ_TYPE(U<16>, decltype(0x10000000000000000_UI));
    EXPECT_EQ_TYPE(U<16>, decltype(0x7fffffffffffffffffffffffffffffff_UI));
    EXPECT_EQ_TYPE(U<16>, decltype(0x80000000000000000000000000000000_UI));
    EXPECT_EQ_TYPE(U<16>, decltype(0xffffffffffffffffffffffffffffffff_UI));
    EXPECT_EQ_TYPE(U<16>, decltype(18446744073709551616_UI));
    EXPECT_EQ_TYPE(U<16>, decltype(170141183460469231731687303715884105727_UI));
    EXPECT_EQ_TYPE(U<16>, decltype(170141183460469231731687303715884105728_UI));
    EXPECT_EQ_TYPE(U<16>, decltype(340282366920938463463374607431768211455_UI));
    EXPECT_EQ_TYPE(U<16>, decltype(02000000000000000000000_UI));
    EXPECT_EQ_TYPE(U<16>, decltype(01777777777777777777777777777777777777777777_UI));
    EXPECT_EQ_TYPE(U<16>, decltype(02000000000000000000000000000000000000000000_UI));
    EXPECT_EQ_TYPE(U<16>, decltype(03777777777777777777777777777777777777777777_UI));
    EXPECT_EQ_TYPE(U<16>, decltype(0b10000000000000000000000000000000000000000000000000000000000000000_UI));
    EXPECT_EQ_TYPE(U<16>, decltype(0b1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111_UI));
    EXPECT_EQ_TYPE(U<16>, decltype(0b10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000_UI));
    EXPECT_EQ_TYPE(U<16>, decltype(0b11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111_UI));
}

TEST(Literals, signed_0_return_type) {
    EXPECT_EQ_TYPE(S<1>, decltype(0_SI));
    EXPECT_EQ_TYPE(S<1>, decltype(0x0_SI));
    EXPECT_EQ_TYPE(S<1>, decltype(00_SI));
    EXPECT_EQ_TYPE(S<1>, decltype(0b0_SI));
    EXPECT_EQ_TYPE(S<1>, decltype(0x00000000_SI));
    EXPECT_EQ_TYPE(S<1>, decltype(000000000_SI));
    EXPECT_EQ_TYPE(S<1>, decltype(0b00000000_SI));
}

TEST(Literals, signed_1_return_type) {
    EXPECT_EQ_TYPE(S<1>, decltype(0x1_SI));
    EXPECT_EQ_TYPE(S<1>, decltype(0x7f_SI));
    EXPECT_EQ_TYPE(S<1>, decltype(1_SI));
    EXPECT_EQ_TYPE(S<1>, decltype(127_SI));
    EXPECT_EQ_TYPE(S<1>, decltype(01_SI));
    EXPECT_EQ_TYPE(S<1>, decltype(0177_SI));
    EXPECT_EQ_TYPE(S<1>, decltype(0b1_SI));
    EXPECT_EQ_TYPE(S<1>, decltype(0b1111111_SI));
}

TEST(Literals, signed_2_return_type) {
    EXPECT_EQ_TYPE(S<2>, decltype(0x80_SI));
    EXPECT_EQ_TYPE(S<2>, decltype(0xff_SI));
    EXPECT_EQ_TYPE(S<2>, decltype(128_SI));
    EXPECT_EQ_TYPE(S<2>, decltype(255_SI));
    EXPECT_EQ_TYPE(S<2>, decltype(0200_SI));
    EXPECT_EQ_TYPE(S<2>, decltype(0377_SI));
    EXPECT_EQ_TYPE(S<2>, decltype(0b10000000_SI));
    EXPECT_EQ_TYPE(S<2>, decltype(0b11111111_SI));

    EXPECT_EQ_TYPE(S<2>, decltype(0x100_SI));
    EXPECT_EQ_TYPE(S<2>, decltype(0x7fff_SI));
    EXPECT_EQ_TYPE(S<2>, decltype(256_SI));
    EXPECT_EQ_TYPE(S<2>, decltype(32767_SI));
    EXPECT_EQ_TYPE(S<2>, decltype(0400_SI));
    EXPECT_EQ_TYPE(S<2>, decltype(077777_SI));
    EXPECT_EQ_TYPE(S<2>, decltype(0b100000000_SI));
    EXPECT_EQ_TYPE(S<2>, decltype(0b111111111111111_SI));
}

TEST(Literals, signed_4_return_type) {
    EXPECT_EQ_TYPE(S<4>, decltype(0x8000_SI));
    EXPECT_EQ_TYPE(S<4>, decltype(0xffff_SI));
    EXPECT_EQ_TYPE(S<4>, decltype(32768_SI));
    EXPECT_EQ_TYPE(S<4>, decltype(65535_SI));
    EXPECT_EQ_TYPE(S<4>, decltype(0100000_SI));
    EXPECT_EQ_TYPE(S<4>, decltype(0177777_SI));
    EXPECT_EQ_TYPE(S<4>, decltype(0b1000000000000000_SI));
    EXPECT_EQ_TYPE(S<4>, decltype(0b1111111111111111_SI));

    EXPECT_EQ_TYPE(S<4>, decltype(0x10000_SI));
    EXPECT_EQ_TYPE(S<4>, decltype(0x7fffffff_SI));
    EXPECT_EQ_TYPE(S<4>, decltype(65536_SI));
    EXPECT_EQ_TYPE(S<4>, decltype(2147483647_SI));
    EXPECT_EQ_TYPE(S<4>, decltype(0200000_SI));
    EXPECT_EQ_TYPE(S<4>, decltype(017777777777_SI));
    EXPECT_EQ_TYPE(S<4>, decltype(0b10000000000000000_SI));
    EXPECT_EQ_TYPE(S<4>, decltype(0b1111111111111111111111111111111_SI));
}

TEST(Literals, signed_8_return_type) {
    EXPECT_EQ_TYPE(S<8>, decltype(0x80000000_SI));
    EXPECT_EQ_TYPE(S<8>, decltype(0xffffffff_SI));
    EXPECT_EQ_TYPE(S<8>, decltype(2147483648_SI));
    EXPECT_EQ_TYPE(S<8>, decltype(4294967295_SI));
    EXPECT_EQ_TYPE(S<8>, decltype(020000000000_SI));
    EXPECT_EQ_TYPE(S<8>, decltype(037777777777_SI));
    EXPECT_EQ_TYPE(S<8>, decltype(0b10000000000000000000000000000000_SI));
    EXPECT_EQ_TYPE(S<8>, decltype(0b11111111111111111111111111111111_SI));

    EXPECT_EQ_TYPE(S<8>, decltype(0x100000000_SI));
    EXPECT_EQ_TYPE(S<8>, decltype(0x7fffffffffffffff_SI));
    EXPECT_EQ_TYPE(S<8>, decltype(4294967296_SI));
    EXPECT_EQ_TYPE(S<8>, decltype(9223372036854775807_SI));
    EXPECT_EQ_TYPE(S<8>, decltype(040000000000_SI));
    EXPECT_EQ_TYPE(S<8>, decltype(0777777777777777777777_SI));
    EXPECT_EQ_TYPE(S<8>, decltype(0b100000000000000000000000000000000_SI));
    EXPECT_EQ_TYPE(S<8>, decltype(0b111111111111111111111111111111111111111111111111111111111111111_SI));
}

TEST(Literals, signed_16_return_type) {
    EXPECT_EQ_TYPE(S<16>, decltype(0x8000000000000000_SI));
    EXPECT_EQ_TYPE(S<16>, decltype(0xffffffffffffffff_SI));
    EXPECT_EQ_TYPE(S<16>, decltype(9223372036854775808_SI));
    EXPECT_EQ_TYPE(S<16>, decltype(18446744073709551615_SI));
    EXPECT_EQ_TYPE(S<16>, decltype(01000000000000000000000_SI));
    EXPECT_EQ_TYPE(S<16>, decltype(01777777777777777777777_SI));
    EXPECT_EQ_TYPE(S<16>, decltype(0b1000000000000000000000000000000000000000000000000000000000000000_SI));
    EXPECT_EQ_TYPE(S<16>, decltype(0b1111111111111111111111111111111111111111111111111111111111111111_SI));

    EXPECT_EQ_TYPE(S<16>, decltype(0x10000000000000000_SI));
    EXPECT_EQ_TYPE(S<16>, decltype(0x7fffffffffffffffffffffffffffffff_SI));
    EXPECT_EQ_TYPE(S<16>, decltype(18446744073709551616_SI));
    EXPECT_EQ_TYPE(S<16>, decltype(170141183460469231731687303715884105727_SI));
    EXPECT_EQ_TYPE(S<16>, decltype(02000000000000000000000_SI));
    EXPECT_EQ_TYPE(S<16>, decltype(01777777777777777777777777777777777777777777_SI));
    EXPECT_EQ_TYPE(S<16>, decltype(0b10000000000000000000000000000000000000000000000000000000000000000_SI));
    EXPECT_EQ_TYPE(S<16>, decltype(0b1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111_SI));
}

TEST(Literals, signed_32_return_type) {
    EXPECT_EQ_TYPE(S<32>, decltype(0x80000000000000000000000000000000_SI));
    EXPECT_EQ_TYPE(S<32>, decltype(0xffffffffffffffffffffffffffffffff_SI));
    EXPECT_EQ_TYPE(S<32>, decltype(170141183460469231731687303715884105728_SI));
    EXPECT_EQ_TYPE(S<32>, decltype(340282366920938463463374607431768211455_SI));
    EXPECT_EQ_TYPE(S<32>, decltype(02000000000000000000000000000000000000000000_SI));
    EXPECT_EQ_TYPE(S<32>, decltype(03777777777777777777777777777777777777777777_SI));
    EXPECT_EQ_TYPE(S<32>, decltype(0b10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000_SI));
    EXPECT_EQ_TYPE(S<32>, decltype(0b11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111_SI));
}

TEST(Literals, unsigned_0) {
    EXPECT_EQ(U<1>::ZERO(), 0_UI);
    EXPECT_EQ(U<1>::ZERO(), 0x0_UI);
    EXPECT_EQ(U<1>::ZERO(), 00_UI);
    EXPECT_EQ(U<1>::ZERO(), 0b0_UI);
    EXPECT_EQ(U<1>::ZERO(), 0x00000000_UI);
    EXPECT_EQ(U<1>::ZERO(), 000000000_UI);
    EXPECT_EQ(U<1>::ZERO(), 0b00000000_UI);
}

TEST(Literals, unsigned_1) {
    EXPECT_EQ(U<1>(1U), 0x1_UI);
    EXPECT_EQ(U<1>(127U), 0x7f_UI);
    EXPECT_EQ(U<1>(128U), 0x80_UI);
    EXPECT_EQ(U<1>(255U), 0xff_UI);
    EXPECT_EQ(U<1>(1U), 1_UI);
    EXPECT_EQ(U<1>(127U), 127_UI);
    EXPECT_EQ(U<1>(128U), 128_UI);
    EXPECT_EQ(U<1>(255U), 255_UI);
    EXPECT_EQ(U<1>(1U), 01_UI);
    EXPECT_EQ(U<1>(127U), 0177_UI);
    EXPECT_EQ(U<1>(128U), 0200_UI);
    EXPECT_EQ(U<1>(255U), 0377_UI);
    EXPECT_EQ(U<1>(1U), 0b1_UI);
    EXPECT_EQ(U<1>(127U), 0b1111111_UI);
    EXPECT_EQ(U<1>(128U), 0b10000000_UI);
    EXPECT_EQ(U<1>(255U), 0b11111111_UI);
}

TEST(Literals, unsigned_2) {
    EXPECT_EQ(U<2>(256U), 0x100_UI);
    EXPECT_EQ(U<2>(32767U), 0x7fff_UI);
    EXPECT_EQ(U<2>(32768U), 0x8000_UI);
    EXPECT_EQ(U<2>(65535U), 0xffff_UI);
    EXPECT_EQ(U<2>(256U), 256_UI);
    EXPECT_EQ(U<2>(32767U), 32767_UI);
    EXPECT_EQ(U<2>(32768U), 32768_UI);
    EXPECT_EQ(U<2>(65535U), 65535_UI);
    EXPECT_EQ(U<2>(256U), 0400_UI);
    EXPECT_EQ(U<2>(32767U), 077777_UI);
    EXPECT_EQ(U<2>(32768U), 0100000_UI);
    EXPECT_EQ(U<2>(65535U), 0177777_UI);
    EXPECT_EQ(U<2>(256U), 0b100000000_UI);
    EXPECT_EQ(U<2>(32767U), 0b111111111111111_UI);
    EXPECT_EQ(U<2>(32768U), 0b1000000000000000_UI);
    EXPECT_EQ(U<2>(65535U), 0b1111111111111111_UI);
}

TEST(Literals, unsigned_4) {
    EXPECT_EQ(U<4>(65536UL), 0x10000_UI);
    EXPECT_EQ(U<4>(2147483647UL), 0x7fffffff_UI);
    EXPECT_EQ(U<4>(2147483648UL), 0x80000000_UI);
    EXPECT_EQ(U<4>(4294967295UL), 0xffffffff_UI);
    EXPECT_EQ(U<4>(65536UL), 65536_UI);
    EXPECT_EQ(U<4>(2147483647UL), 2147483647_UI);
    EXPECT_EQ(U<4>(2147483648UL), 2147483648_UI);
    EXPECT_EQ(U<4>(4294967295UL), 4294967295_UI);
    EXPECT_EQ(U<4>(65536UL), 0200000_UI);
    EXPECT_EQ(U<4>(2147483647UL), 017777777777_UI);
    EXPECT_EQ(U<4>(2147483648UL), 020000000000_UI);
    EXPECT_EQ(U<4>(4294967295UL), 037777777777_UI);
    EXPECT_EQ(U<4>(65536UL), 0b10000000000000000_UI);
    EXPECT_EQ(U<4>(2147483647UL), 0b1111111111111111111111111111111_UI);
    EXPECT_EQ(U<4>(2147483648UL), 0b10000000000000000000000000000000_UI);
    EXPECT_EQ(U<4>(4294967295UL), 0b11111111111111111111111111111111_UI);
}

TEST(Literals, unsigned_8) {
    EXPECT_EQ(U<8>(4294967296ULL), 0x100000000_UI);
    EXPECT_EQ(U<8>(9223372036854775807ULL), 0x7fffffffffffffff_UI);
    EXPECT_EQ(U<8>(9223372036854775808ULL), 0x8000000000000000_UI);
    EXPECT_EQ(U<8>(18446744073709551615ULL), 0xffffffffffffffff_UI);
    EXPECT_EQ(U<8>(4294967296ULL), 4294967296_UI);
    EXPECT_EQ(U<8>(9223372036854775807ULL), 9223372036854775807_UI);
    EXPECT_EQ(U<8>(9223372036854775808ULL), 9223372036854775808_UI);
    EXPECT_EQ(U<8>(18446744073709551615ULL), 18446744073709551615_UI);
    EXPECT_EQ(U<8>(4294967296ULL), 040000000000_UI);
    EXPECT_EQ(U<8>(9223372036854775807ULL), 0777777777777777777777_UI);
    EXPECT_EQ(U<8>(9223372036854775808ULL), 01000000000000000000000_UI);
    EXPECT_EQ(U<8>(18446744073709551615ULL), 01777777777777777777777_UI);
    EXPECT_EQ(U<8>(4294967296ULL), 0b100000000000000000000000000000000_UI);
    EXPECT_EQ(U<8>(9223372036854775807ULL), 0b111111111111111111111111111111111111111111111111111111111111111_UI);
    EXPECT_EQ(U<8>(9223372036854775808ULL), 0b1000000000000000000000000000000000000000000000000000000000000000_UI);
    EXPECT_EQ(U<8>(18446744073709551615ULL), 0b1111111111111111111111111111111111111111111111111111111111111111_UI);
}

TEST(Literals, unsigned_16) {
    EXPECT_EQ(U<16>(0ULL, 1ULL), 0x10000000000000000_UI);
    EXPECT_EQ(U<16>(0xffffffffffffffffULL, 0x7fffffffffffffffULL), 0x7fffffffffffffffffffffffffffffff_UI);
    EXPECT_EQ(U<16>(0ULL, 0x8000000000000000ULL), 0x80000000000000000000000000000000_UI);
    EXPECT_EQ(U<16>(0xffffffffffffffffULL, 0xffffffffffffffffULL), 0xffffffffffffffffffffffffffffffff_UI);
    EXPECT_EQ(U<16>(0ULL, 1ULL), 18446744073709551616_UI);
    EXPECT_EQ(U<16>(0xffffffffffffffffULL, 0x7fffffffffffffffULL), 170141183460469231731687303715884105727_UI);
    EXPECT_EQ(U<16>(0ULL, 0x8000000000000000ULL), 170141183460469231731687303715884105728_UI);
    EXPECT_EQ(U<16>(0xffffffffffffffffULL, 0xffffffffffffffffULL), 340282366920938463463374607431768211455_UI);
    EXPECT_EQ(U<16>(0ULL, 1ULL), 02000000000000000000000_UI);
    EXPECT_EQ(U<16>(0xffffffffffffffffULL, 0x7fffffffffffffffULL), 01777777777777777777777777777777777777777777_UI);
    EXPECT_EQ(U<16>(0ULL, 0x8000000000000000ULL), 02000000000000000000000000000000000000000000_UI);
    EXPECT_EQ(U<16>(0xffffffffffffffffULL, 0xffffffffffffffffULL), 03777777777777777777777777777777777777777777_UI);
    EXPECT_EQ(U<16>(0ULL, 1ULL), 0b10000000000000000000000000000000000000000000000000000000000000000_UI);
    EXPECT_EQ(U<16>(0xffffffffffffffffULL, 0x7fffffffffffffffULL), 0b1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111_UI);
    EXPECT_EQ(U<16>(0ULL, 0x8000000000000000ULL), 0b10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000_UI);
    EXPECT_EQ(U<16>(0xffffffffffffffffULL, 0xffffffffffffffffULL), 0b11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111_UI);
}

TEST(Literals, signed_0) {
    EXPECT_EQ(S<1>::ZERO(), 0_SI);
    EXPECT_EQ(S<1>::ZERO(), 0x0_SI);
    EXPECT_EQ(S<1>::ZERO(), 00_SI);
    EXPECT_EQ(S<1>::ZERO(), 0b0_SI);
    EXPECT_EQ(S<1>::ZERO(), 0x00000000_SI);
    EXPECT_EQ(S<1>::ZERO(), 000000000_SI);
    EXPECT_EQ(S<1>::ZERO(), 0b00000000_SI);
}

TEST(Literals, signed_1) {
    EXPECT_EQ(S<1>(1U), 0x1_SI);
    EXPECT_EQ(S<1>(127U), 0x7f_SI);
    EXPECT_EQ(S<1>(1U), 1_SI);
    EXPECT_EQ(S<1>(127U), 127_SI);
    EXPECT_EQ(S<1>(1U), 01_SI);
    EXPECT_EQ(S<1>(127U), 0177_SI);
    EXPECT_EQ(S<1>(1U), 0b1_SI);
    EXPECT_EQ(S<1>(127U), 0b1111111_SI);
}

TEST(Literals, signed_2) {
    EXPECT_EQ(S<2>(128U), 0x80_SI);
    EXPECT_EQ(S<2>(255U), 0xff_SI);
    EXPECT_EQ(S<2>(128U), 128_SI);
    EXPECT_EQ(S<2>(255U), 255_SI);
    EXPECT_EQ(S<2>(128U), 0200_SI);
    EXPECT_EQ(S<2>(255U), 0377_SI);
    EXPECT_EQ(S<2>(128U), 0b10000000_SI);
    EXPECT_EQ(S<2>(255U), 0b11111111_SI);

    EXPECT_EQ(S<2>(256U), 0x100_SI);
    EXPECT_EQ(S<2>(32767U), 0x7fff_SI);
    EXPECT_EQ(S<2>(256U), 256_SI);
    EXPECT_EQ(S<2>(32767U), 32767_SI);
    EXPECT_EQ(S<2>(256U), 0400_SI);
    EXPECT_EQ(S<2>(32767U), 077777_SI);
    EXPECT_EQ(S<2>(256U), 0b100000000_SI);
    EXPECT_EQ(S<2>(32767U), 0b111111111111111_SI);
}

TEST(Literals, signed_4) {
    EXPECT_EQ(S<4>(32768U), 0x8000_SI);
    EXPECT_EQ(S<4>(65535U), 0xffff_SI);
    EXPECT_EQ(S<4>(32768U), 32768_SI);
    EXPECT_EQ(S<4>(65535U), 65535_SI);
    EXPECT_EQ(S<4>(32768U), 0100000_SI);
    EXPECT_EQ(S<4>(65535U), 0177777_SI);
    EXPECT_EQ(S<4>(32768U), 0b1000000000000000_SI);
    EXPECT_EQ(S<4>(65535U), 0b1111111111111111_SI);

    EXPECT_EQ(S<4>(65536UL), 0x10000_SI);
    EXPECT_EQ(S<4>(2147483647UL), 0x7fffffff_SI);
    EXPECT_EQ(S<4>(65536UL), 65536_SI);
    EXPECT_EQ(S<4>(2147483647UL), 2147483647_SI);
    EXPECT_EQ(S<4>(65536UL), 0200000_SI);
    EXPECT_EQ(S<4>(2147483647UL), 017777777777_SI);
    EXPECT_EQ(S<4>(65536UL), 0b10000000000000000_SI);
    EXPECT_EQ(S<4>(2147483647UL), 0b1111111111111111111111111111111_SI);
}

TEST(Literals, signed_8) {
    EXPECT_EQ(S<8>(2147483648UL), 0x80000000_SI);
    EXPECT_EQ(S<8>(4294967295UL), 0xffffffff_SI);
    EXPECT_EQ(S<8>(2147483648UL), 2147483648_SI);
    EXPECT_EQ(S<8>(4294967295UL), 4294967295_SI);
    EXPECT_EQ(S<8>(2147483648UL), 020000000000_SI);
    EXPECT_EQ(S<8>(4294967295UL), 037777777777_SI);
    EXPECT_EQ(S<8>(2147483648UL), 0b10000000000000000000000000000000_SI);
    EXPECT_EQ(S<8>(4294967295UL), 0b11111111111111111111111111111111_SI);

    EXPECT_EQ(S<8>(4294967296ULL), 0x100000000_SI);
    EXPECT_EQ(S<8>(9223372036854775807ULL), 0x7fffffffffffffff_SI);
    EXPECT_EQ(S<8>(4294967296ULL), 4294967296_SI);
    EXPECT_EQ(S<8>(9223372036854775807ULL), 9223372036854775807_SI);
    EXPECT_EQ(S<8>(4294967296ULL), 040000000000_SI);
    EXPECT_EQ(S<8>(9223372036854775807ULL), 0777777777777777777777_SI);
    EXPECT_EQ(S<8>(4294967296ULL), 0b100000000000000000000000000000000_SI);
    EXPECT_EQ(S<8>(9223372036854775807ULL), 0b111111111111111111111111111111111111111111111111111111111111111_SI);
}

TEST(Literals, signed_16) {
    EXPECT_EQ(S<16>(9223372036854775808ULL), 0x8000000000000000_SI);
    EXPECT_EQ(S<16>(18446744073709551615ULL), 0xffffffffffffffff_SI);
    EXPECT_EQ(S<16>(9223372036854775808ULL), 9223372036854775808_SI);
    EXPECT_EQ(S<16>(18446744073709551615ULL), 18446744073709551615_SI);
    EXPECT_EQ(S<16>(9223372036854775808ULL), 01000000000000000000000_SI);
    EXPECT_EQ(S<16>(18446744073709551615ULL), 01777777777777777777777_SI);
    EXPECT_EQ(S<16>(9223372036854775808ULL), 0b1000000000000000000000000000000000000000000000000000000000000000_SI);
    EXPECT_EQ(S<16>(18446744073709551615ULL), 0b1111111111111111111111111111111111111111111111111111111111111111_SI);

    EXPECT_EQ(S<16>(0ULL, 1ULL), 0x10000000000000000_SI);
    EXPECT_EQ(S<16>(0xffffffffffffffffULL, 0x7fffffffffffffffULL), 0x7fffffffffffffffffffffffffffffff_SI);
    EXPECT_EQ(S<16>(0ULL, 1ULL), 18446744073709551616_SI);
    EXPECT_EQ(S<16>(0xffffffffffffffffULL, 0x7fffffffffffffffULL), 170141183460469231731687303715884105727_SI);
    EXPECT_EQ(S<16>(0ULL, 1ULL), 02000000000000000000000_SI);
    EXPECT_EQ(S<16>(0xffffffffffffffffULL, 0x7fffffffffffffffULL), 01777777777777777777777777777777777777777777_SI);
    EXPECT_EQ(S<16>(0ULL, 1ULL), 0b10000000000000000000000000000000000000000000000000000000000000000_SI);
    EXPECT_EQ(S<16>(0xffffffffffffffffULL, 0x7fffffffffffffffULL), 0b1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111_SI);
}

TEST(Literals, signed_32) {
    EXPECT_EQ(S<32>(U<16>(0ULL, 0x8000000000000000ULL)), 0x80000000000000000000000000000000_SI);
    EXPECT_EQ(S<32>(U<16>(0xffffffffffffffffULL, 0xffffffffffffffffULL)), 0xffffffffffffffffffffffffffffffff_SI);
    EXPECT_EQ(S<32>(U<16>(0ULL, 0x8000000000000000ULL)), 170141183460469231731687303715884105728_SI);
    EXPECT_EQ(S<32>(U<16>(0xffffffffffffffffULL, 0xffffffffffffffffULL)), 340282366920938463463374607431768211455_SI);
    EXPECT_EQ(S<32>(U<16>(0ULL, 0x8000000000000000ULL)), 02000000000000000000000000000000000000000000_SI);
    EXPECT_EQ(S<32>(U<16>(0xffffffffffffffffULL, 0xffffffffffffffffULL)), 03777777777777777777777777777777777777777777_SI);
    EXPECT_EQ(S<32>(U<16>(0ULL, 0x8000000000000000ULL)), 0b10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000_SI);
    EXPECT_EQ(S<32>(U<16>(0xffffffffffffffffULL, 0xffffffffffffffffULL)), 0b11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111_SI);
}
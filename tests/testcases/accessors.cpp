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

TEST(Accessors, native_unsigned_return_type) {
    {
        U<1> value = U<1>::ZERO();
        ASSERT_EQ_TYPE(uint8_t&, decltype(value.uvalue()));
        const U<1> cvalue = U<1>::ZERO();
        ASSERT_EQ_TYPE(const uint8_t&, decltype(cvalue.uvalue()));
    }
    {
        U<2> value = U<2>::ZERO();
        ASSERT_EQ_TYPE(uint16_t&, decltype(value.uvalue()));
        const U<2> cvalue = U<2>::ZERO();
        ASSERT_EQ_TYPE(const uint16_t&, decltype(cvalue.uvalue()));
    }
    {
        U<4> value = U<4>::ZERO();
        ASSERT_EQ_TYPE(uint32_t&, decltype(value.uvalue()));
        const U<4> cvalue = U<4>::ZERO();
        ASSERT_EQ_TYPE(const uint32_t&, decltype(cvalue.uvalue()));
    }
    {
        U<8> value = U<8>::ZERO();
        ASSERT_EQ_TYPE(uint64_t&, decltype(value.uvalue()));
        const U<8> cvalue = U<8>::ZERO();
        ASSERT_EQ_TYPE(const uint64_t&, decltype(cvalue.uvalue()));
    }
}

TEST(Accessors, native_signed_return_type) {
    {
        S<1> value = S<1>::ZERO();
        ASSERT_EQ_TYPE(uint8_t&, decltype(value.uvalue()));
        const S<1> cvalue = S<1>::ZERO();
        ASSERT_EQ_TYPE(const uint8_t&, decltype(cvalue.uvalue()));
    }
    {
        S<2> value = S<2>::ZERO();
        ASSERT_EQ_TYPE(uint16_t&, decltype(value.uvalue()));
        const S<2> cvalue = S<2>::ZERO();
        ASSERT_EQ_TYPE(const uint16_t&, decltype(cvalue.uvalue()));
    }
    {
        S<4> value = S<4>::ZERO();
        ASSERT_EQ_TYPE(uint32_t&, decltype(value.uvalue()));
        const S<4> cvalue = S<4>::ZERO();
        ASSERT_EQ_TYPE(const uint32_t&, decltype(cvalue.uvalue()));
    }
    {
        S<8> value = S<8>::ZERO();
        ASSERT_EQ_TYPE(uint64_t&, decltype(value.uvalue()));
        const S<8> cvalue = S<8>::ZERO();
        ASSERT_EQ_TYPE(const uint64_t&, decltype(cvalue.uvalue()));
    }
}

TEST(Accessors, pow2_unsigned_return_type) {
    U<16> value = U<16>::ZERO();
    ASSERT_EQ_TYPE(U<8>&, decltype(value.ulow()));
    ASSERT_EQ_TYPE(U<8>&, decltype(value.uhigh()));
    const U<16> cvalue = U<16>::ZERO();
    ASSERT_EQ_TYPE(const U<8>&, decltype(cvalue.ulow()));
    ASSERT_EQ_TYPE(const U<8>&, decltype(cvalue.uhigh()));
}

TEST(Accessors, pow2_signed_return_type) {
    S<16> value = S<16>::ZERO();
    ASSERT_EQ_TYPE(U<8>&, decltype(value.ulow()));
    ASSERT_EQ_TYPE(U<8>&, decltype(value.uhigh()));
    const S<16> cvalue = S<16>::ZERO();
    ASSERT_EQ_TYPE(const U<8>&, decltype(cvalue.ulow()));
    ASSERT_EQ_TYPE(const U<8>&, decltype(cvalue.uhigh()));
}

TEST(Accessors, native_unsigned) {
    U<8> value = U<8>(0xfedcba9876543210ULL);
    EXPECT_EQ(value.uvalue(), 0xfedcba9876543210ULL);
    value.uvalue() = 0x0123456789abcdefULL;
    EXPECT_EQ(value.uvalue(), 0x0123456789abcdefULL);
}

TEST(Accessors, native_signed) {
    S<8> value = S<8>(0xfedcba9876543210ULL);
    EXPECT_EQ(value.uvalue(), 0xfedcba9876543210ULL);
    value.uvalue() = 0x0123456789abcdefULL;
    EXPECT_EQ(value.uvalue(), 0x0123456789abcdefULL);
}

TEST(Accessors, pow2_unsigned) {
    U<16> value = U<16>(0xfedcba9876543210ULL, 0x0123456789abcdefULL);
    EXPECT_EQ(value.ulow(), 0xfedcba9876543210ULL);
    EXPECT_EQ(value.uhigh(), 0x0123456789abcdefULL);
    value.ulow() = 0x0123456789abcdefULL;
    value.uhigh() = 0xfedcba9876543210ULL;
    EXPECT_EQ(value.ulow(), 0x0123456789abcdefULL);
    EXPECT_EQ(value.uhigh(), 0xfedcba9876543210ULL);
}

TEST(Accessors, pow2_signed) {
    S<16> value = S<16>(0xfedcba9876543210ULL, 0x0123456789abcdefULL);
    EXPECT_EQ(value.ulow(), 0xfedcba9876543210ULL);
    EXPECT_EQ(value.uhigh(), 0x0123456789abcdefULL);
    value.ulow() = 0x0123456789abcdefULL;
    value.uhigh() = 0xfedcba9876543210ULL;
    EXPECT_EQ(value.ulow(), 0x0123456789abcdefULL);
    EXPECT_EQ(value.uhigh(), 0xfedcba9876543210ULL);
}

#include <gtest/gtest.h>
#include <Integer.hpp>
#include <type_traits>

using namespace JIO;

template<size_t size>
using U = Integer<size, false>;
template<size_t size>
using S = Integer<size, true>;

TEST(Constructors, case_0_native_unsigned) {
    constexpr U<8> value = U<8>::ONE();
    constexpr U<8> copy = value;

    EXPECT_EQ(U<8>(), 0);
    EXPECT_EQ(value, copy);
    EXPECT_EQ(U<8>(std::move(value)), copy);
    EXPECT_EQ(value, 1);
}

TEST(Constructors, case_0_native_signed) {
    constexpr S<8> value = S<8>::ONE();
    constexpr S<8> copy = value;

    EXPECT_EQ(S<8>(), 0);
    EXPECT_EQ(value, copy);
    EXPECT_EQ(S<8>(std::move(value)), copy);
    EXPECT_EQ(value, 1);
}

TEST(Constructors, case_0_pow2_unsigned) {
    constexpr U<16> value = U<16>::ONE();
    constexpr U<16> copy = value;

    EXPECT_EQ(U<16>(), 0);
    EXPECT_EQ(value, copy);
    EXPECT_EQ(U<16>(std::move(value)), copy);
    EXPECT_EQ(value, 1);
}

TEST(Constructors, case_0_pow2_signed) {
    constexpr S<16> value = S<16>::ONE();
    constexpr S<16> copy = value;

    EXPECT_EQ(S<16>(), 0);
    EXPECT_EQ(value, copy);
    EXPECT_EQ(S<16>(std::move(value)), copy);
    EXPECT_EQ(value, 1);
}

TEST(Constructors, case_1_unsigned) {
    constexpr U<1> v1 = U<1>::ZERO();
    EXPECT_EQ(v1, 0);
    constexpr U<2> v2 = v1;
    EXPECT_EQ(v2, 0);
    constexpr U<4> v3 = v2;
    EXPECT_EQ(v3, 0);
    constexpr U<8> v4 = v3;
    EXPECT_EQ(v4, 0);
    constexpr U<16> v5 = v4;
    EXPECT_EQ(v5, 0);
}

TEST(Constructors, case_1_signed) {
    constexpr S<1> v1 = S<1>::ZERO();
    EXPECT_EQ(v1, 0);
    constexpr S<2> v2 = v1;
    EXPECT_EQ(v2, 0);
    constexpr S<4> v3 = v2;
    EXPECT_EQ(v3, 0);
    constexpr S<8> v4 = v3;
    EXPECT_EQ(v4, 0);
    constexpr S<16> v5 = v4;
    EXPECT_EQ(v5, 0);
}

TEST(Constructors, case_2_unsigned) {
    constexpr U<1> v1 = U<1>::ONE();
    EXPECT_EQ(v1, 1);
    constexpr U<2> v2 = v1;
    EXPECT_EQ(v2, 1);
    constexpr U<4> v3 = v2;
    EXPECT_EQ(v3, 1);
    constexpr U<8> v4 = v3;
    EXPECT_EQ(v4, 1);
    constexpr U<16> v5 = v4;
    EXPECT_EQ(v5, 1);
}

TEST(Constructors, case_2_signed) {
    constexpr S<1> v1 = S<1>::ONE();
    EXPECT_EQ(v1, 1);
    constexpr S<2> v2 = v1;
    EXPECT_EQ(v2, 1);
    constexpr S<4> v3 = v2;
    EXPECT_EQ(v3, 1);
    constexpr S<8> v4 = v3;
    EXPECT_EQ(v4, 1);
    constexpr S<16> v5 = v4;
    EXPECT_EQ(v5, 1);
}

TEST(Constructors, case_3_unsigned) {
    constexpr S<1> v0 = S<1>(-1);
    constexpr U<1> v1 = v0;
    EXPECT_EQ(v1, uint8_t(0xffU));
    constexpr U<2> v2 = v0;
    EXPECT_EQ(v2, uint16_t(0xffffU));
    constexpr U<4> v3 = v0;
    EXPECT_EQ(v3, uint32_t(0xffffffffUL));
    constexpr U<8> v4 = v0;
    EXPECT_EQ(v4, uint64_t(0xffffffffffffffffULL));
    constexpr U<16> v5 = v0;
    EXPECT_EQ(v5.ulow(), uint64_t(0xffffffffffffffffULL));
    EXPECT_EQ(v5.uhigh(), uint64_t(0xffffffffffffffffULL));
}

TEST(Constructors, case_3_signed) {
    constexpr S<1> v1 = S<1>(-1);
    EXPECT_EQ(v1, int8_t(-1));
    constexpr S<2> v2 = v1;
    EXPECT_EQ(v2, int16_t(-1));
    constexpr S<4> v3 = v1;
    EXPECT_EQ(v3, int32_t(-1L));
    constexpr S<8> v4 = v1;
    EXPECT_EQ(v4, int64_t(-1LL));
    constexpr S<16> v5 = v1;
    EXPECT_EQ(v5.ulow(), uint64_t(0xffffffffffffffffULL));
    EXPECT_EQ(v5.uhigh(), uint64_t(0xffffffffffffffffULL));
}

TEST(Constructors, case_4_unsigned) {
    constexpr int8_t v0 = int8_t(-1);
    constexpr U<1> v1 = v0;
    EXPECT_EQ(v1, uint8_t(0xffU));
    constexpr U<2> v2 = v0;
    EXPECT_EQ(v2, uint16_t(0xffffU));
    constexpr U<4> v3 = v0;
    EXPECT_EQ(v3, uint32_t(0xffffffffUL));
    constexpr U<8> v4 = v0;
    EXPECT_EQ(v4, uint64_t(0xffffffffffffffffULL));
    constexpr U<16> v5 = v0;
    EXPECT_EQ(v5.ulow(), uint64_t(0xffffffffffffffffULL));
    EXPECT_EQ(v5.uhigh(), uint64_t(0xffffffffffffffffULL));
}

TEST(Constructors, case_4_signed) {
    constexpr int8_t v0 = int8_t(-1);
    constexpr S<1> v1 = v0;
    EXPECT_EQ(v1, int8_t(-1));
    constexpr S<2> v2 = v0;
    EXPECT_EQ(v2, int16_t(-1));
    constexpr S<4> v3 = v0;
    EXPECT_EQ(v3, int32_t(-1L));
    constexpr S<8> v4 = v0;
    EXPECT_EQ(v4, int64_t(-1LL));
    constexpr S<16> v5 = v0;
    EXPECT_EQ(v5.ulow(), uint64_t(0xffffffffffffffffULL));
    EXPECT_EQ(v5.uhigh(), uint64_t(0xffffffffffffffffULL));
}

TEST(Constructors, case_5_unsigned) {
    constexpr int16_t v0 = int16_t(-1);
    constexpr U<1> v1 = U<1>(v0);
    EXPECT_EQ(v1, uint8_t(0xffU));
    constexpr U<2> v2 = v0;
    EXPECT_EQ(v2, uint16_t(0xffffU));
    constexpr U<4> v3 = v0;
    EXPECT_EQ(v3, uint32_t(0xffffffffUL));
    constexpr U<8> v4 = v0;
    EXPECT_EQ(v4, uint64_t(0xffffffffffffffffULL));
    constexpr U<16> v5 = v0;
    EXPECT_EQ(v5.ulow(), uint64_t(0xffffffffffffffffULL));
    EXPECT_EQ(v5.uhigh(), uint64_t(0xffffffffffffffffULL));
}

TEST(Constructors, case_5_signed) {
    constexpr int16_t v0 = int16_t(-1);
    constexpr S<1> v1 = S<1>(v0);
    EXPECT_EQ(v1, int8_t(-1));
    constexpr S<2> v2 = v0;
    EXPECT_EQ(v2, int16_t(-1));
    constexpr S<4> v3 = v0;
    EXPECT_EQ(v3, int32_t(-1L));
    constexpr S<8> v4 = v0;
    EXPECT_EQ(v4, int64_t(-1LL));
    constexpr S<16> v5 = v0;
    EXPECT_EQ(v5.ulow(), uint64_t(0xffffffffffffffffULL));
    EXPECT_EQ(v5.uhigh(), uint64_t(0xffffffffffffffffULL));
}

TEST(Constructors, case_6_unsigned) {
    constexpr int32_t v0 = -1L;
    constexpr U<1> v1 = U<1>(v0);
    EXPECT_EQ(v1, uint8_t(0xffU));
    constexpr U<2> v2 = U<2>(v0);
    EXPECT_EQ(v2, uint16_t(0xffffU));
    constexpr U<4> v3 = v0;
    EXPECT_EQ(v3, uint32_t(0xffffffffUL));
    constexpr U<8> v4 = v0;
    EXPECT_EQ(v4, uint64_t(0xffffffffffffffffULL));
    constexpr U<16> v5 = v0;
    EXPECT_EQ(v5.ulow(), uint64_t(0xffffffffffffffffULL));
    EXPECT_EQ(v5.uhigh(), uint64_t(0xffffffffffffffffULL));
}

TEST(Constructors, case_6_signed) {
    constexpr int32_t v0 = -1L;
    constexpr S<1> v1 = S<1>(v0);
    EXPECT_EQ(v1, int8_t(-1));
    constexpr S<2> v2 = S<2>(v0);
    EXPECT_EQ(v2, int16_t(-1));
    constexpr S<4> v3 = v0;
    EXPECT_EQ(v3, int32_t(-1L));
    constexpr S<8> v4 = v0;
    EXPECT_EQ(v4, int64_t(-1LL));
    constexpr S<16> v5 = v0;
    EXPECT_EQ(v5.ulow(), uint64_t(0xffffffffffffffffULL));
    EXPECT_EQ(v5.uhigh(), uint64_t(0xffffffffffffffffULL));
}

TEST(Constructors, case_7_unsigned) {
    constexpr int64_t v0 = -1LL;
    constexpr U<1> v1 = U<1>(v0);
    EXPECT_EQ(v1, uint8_t(0xffU));
    constexpr U<2> v2 = U<2>(v0);
    EXPECT_EQ(v2, uint16_t(0xffffU));
    constexpr U<4> v3 = U<4>(v0);
    EXPECT_EQ(v3, uint32_t(0xffffffffUL));
    constexpr U<8> v4 = v0;
    EXPECT_EQ(v4, uint64_t(0xffffffffffffffffULL));
    constexpr U<16> v5 = v0;
    EXPECT_EQ(v5.ulow(), uint64_t(0xffffffffffffffffULL));
    EXPECT_EQ(v5.uhigh(), uint64_t(0xffffffffffffffffULL));
}

TEST(Constructors, case_7_signed) {
    constexpr int64_t v0 = -1LL;
    constexpr S<1> v1 = S<1>(v0);
    EXPECT_EQ(v1, int8_t(-1));
    constexpr S<2> v2 = S<2>(v0);
    EXPECT_EQ(v2, int16_t(-1));
    constexpr S<4> v3 = S<4>(v0);
    EXPECT_EQ(v3, int32_t(-1L));
    constexpr S<8> v4 = v0;
    EXPECT_EQ(v4, int64_t(-1LL));
    constexpr S<16> v5 = v0;
    EXPECT_EQ(v5.ulow(), uint64_t(0xffffffffffffffffULL));
    EXPECT_EQ(v5.uhigh(), uint64_t(0xffffffffffffffffULL));
}

TEST(Constructors, case_8_unsigned) {
    constexpr U<16> v0 = -1LL;
    constexpr U<1> v1 = U<1>(v0);
    EXPECT_EQ(v1, uint8_t(0xffU));
    constexpr U<2> v2 = U<2>(v0);
    EXPECT_EQ(v2, uint16_t(0xffffU));
    constexpr U<4> v3 = U<4>(v0);
    EXPECT_EQ(v3, uint32_t(0xffffffffUL));
    constexpr U<8> v4 = U<8>(v0);
    EXPECT_EQ(v4, uint64_t(0xffffffffffffffffULL));
    constexpr U<16> v5 = v0;
    EXPECT_EQ(v5.ulow(), uint64_t(0xffffffffffffffffULL));
    EXPECT_EQ(v5.uhigh(), uint64_t(0xffffffffffffffffULL));
}

TEST(Constructors, case_8_signed) {
    constexpr S<16> v0 = -1LL;
    constexpr S<1> v1 = S<1>(v0);
    EXPECT_EQ(v1, int8_t(-1));
    constexpr S<2> v2 = S<2>(v0);
    EXPECT_EQ(v2, int16_t(-1));
    constexpr S<4> v3 = S<4>(v0);
    EXPECT_EQ(v3, int32_t(-1L));
    constexpr S<8> v4 = S<8>(v0);
    EXPECT_EQ(v4, int64_t(-1LL));
    S<16> v5 = v0;
    EXPECT_EQ(v5.ulow(), uint64_t(0xffffffffffffffffULL));
    EXPECT_EQ(v5.uhigh(), uint64_t(0xffffffffffffffffULL));
}

TEST(Constructors, case_9_native) {
    constexpr S<8> v1 = 1;
    constexpr U<8> v2 = 2;
    constexpr S<8> cv1 = v2;
    constexpr U<8> cv2 = v1;
    EXPECT_EQ(cv1, 2);
    EXPECT_EQ(cv2, 1);
}

TEST(Constructors, case_9_pow2) {
    constexpr S<16> v1 = 1;
    constexpr U<16> v2 = 2;
    constexpr S<16> cv1 = v2;
    constexpr U<16> cv2 = v1;
    EXPECT_EQ(cv1, 2);
    EXPECT_EQ(cv2, 1);
}

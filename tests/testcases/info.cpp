#include <gtest/gtest.h>
#include <Integer.hpp>
#include <type_traits>

using namespace JIO;

template<size_t size>
using U = Integer<size, false>;
template<size_t size>
using S = Integer<size, true>;

TEST(Info, sizeof_unsigned) {
    ASSERT_EQ(sizeof (U<1>), 1U);
    ASSERT_EQ(sizeof (U<2>), 2U);
    ASSERT_EQ(sizeof (U<4>), 4U);
    ASSERT_EQ(sizeof (U<8>), 8U);
    ASSERT_EQ(sizeof (U<16>), 16U);
    ASSERT_EQ(sizeof (U<32>), 32U);
    ASSERT_EQ(sizeof (U<64>), 64U);
    ASSERT_EQ(sizeof (U<128>), 128U);
}

TEST(Info, sizeof_signed) {
    ASSERT_EQ(sizeof (S<1>), 1U);
    ASSERT_EQ(sizeof (S<2>), 2U);
    ASSERT_EQ(sizeof (S<4>), 4U);
    ASSERT_EQ(sizeof (S<8>), 8U);
    ASSERT_EQ(sizeof (S<16>), 16U);
    ASSERT_EQ(sizeof (S<32>), 32U);
    ASSERT_EQ(sizeof (S<64>), 64U);
    ASSERT_EQ(sizeof (S<128>), 128U);
}

TEST(Info, std_is_pod_unsigned) {
    ASSERT_TRUE(std::is_pod<U < 1 >> ());
    ASSERT_TRUE(std::is_pod<U < 2 >> ());
    ASSERT_TRUE(std::is_pod<U < 4 >> ());
    ASSERT_TRUE(std::is_pod<U < 8 >> ());
    ASSERT_TRUE(std::is_pod<U < 16 >> ());
    ASSERT_TRUE(std::is_pod<U < 32 >> ());
    ASSERT_TRUE(std::is_pod<U < 64 >> ());
    ASSERT_TRUE(std::is_pod<U < 128 >> ());
}

TEST(Info, std_is_pod_signed) {
    ASSERT_TRUE(std::is_pod<S < 1 >> ());
    ASSERT_TRUE(std::is_pod<S < 2 >> ());
    ASSERT_TRUE(std::is_pod<S < 4 >> ());
    ASSERT_TRUE(std::is_pod<S < 8 >> ());
    ASSERT_TRUE(std::is_pod<S < 16 >> ());
    ASSERT_TRUE(std::is_pod<S < 32 >> ());
    ASSERT_TRUE(std::is_pod<S < 64 >> ());
    ASSERT_TRUE(std::is_pod<S < 128 >> ());
}

TEST(Info, std_is_trivial_unsigned) {
    ASSERT_TRUE(std::is_trivial<U < 1 >> ());
    ASSERT_TRUE(std::is_trivial<U < 2 >> ());
    ASSERT_TRUE(std::is_trivial<U < 4 >> ());
    ASSERT_TRUE(std::is_trivial<U < 8 >> ());
    ASSERT_TRUE(std::is_trivial<U < 16 >> ());
    ASSERT_TRUE(std::is_trivial<U < 32 >> ());
    ASSERT_TRUE(std::is_trivial<U < 64 >> ());
    ASSERT_TRUE(std::is_trivial<U < 128 >> ());
}

TEST(Info, std_is_trivial_signed) {
    ASSERT_TRUE(std::is_trivial<S < 1 >> ());
    ASSERT_TRUE(std::is_trivial<S < 2 >> ());
    ASSERT_TRUE(std::is_trivial<S < 4 >> ());
    ASSERT_TRUE(std::is_trivial<S < 8 >> ());
    ASSERT_TRUE(std::is_trivial<S < 16 >> ());
    ASSERT_TRUE(std::is_trivial<S < 32 >> ());
    ASSERT_TRUE(std::is_trivial<S < 64 >> ());
    ASSERT_TRUE(std::is_trivial<S < 128 >> ());
}

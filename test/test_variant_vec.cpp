#include <gtest/gtest.h>

#include "variant_vec.h"
#include <variant>

TEST(TestVariantVec, Construct)
{
    variant_vec<int, float> vv{};

    ASSERT_EQ(vv.size(), 0);
}

TEST(TestVariantVec, EmplaceBackValues)
{
    variant_vec<int, float> vv{};

    vv.emplace_back<int>(10);
    vv.emplace_back<float>(20.0f);
    vv.emplace_back<int>(30);

    ASSERT_EQ(vv.size(), 3);

    ASSERT_TRUE(std::holds_alternative<int>(vv[0]));
    ASSERT_TRUE(std::holds_alternative<float>(vv[1]));
    ASSERT_TRUE(std::holds_alternative<int>(vv[2]));

    ASSERT_EQ(std::get<int>(vv[0]), 10);
    ASSERT_EQ(std::get<float>(vv[1]), 20.0f);
    ASSERT_EQ(std::get<int>(vv[2]), 30);
}

TEST(TestVariantVec, PushBackVariants)
{
    variant_vec<int, float> vv{};

    vv.push_back(10);
    vv.push_back(20.0f);
    vv.push_back(30);

    ASSERT_EQ(vv.size(), 3);

    ASSERT_TRUE(std::holds_alternative<int>(vv[0]));
    ASSERT_TRUE(std::holds_alternative<float>(vv[1]));
    ASSERT_TRUE(std::holds_alternative<int>(vv[2]));

    ASSERT_EQ(std::get<int>(vv[0]), 10);
    ASSERT_EQ(std::get<float>(vv[1]), 20.0f);
    ASSERT_EQ(std::get<int>(vv[2]), 30);
}

TEST(TestVariantVec, IterValues)
{
    variant_vec<int, float> vv{};

    vv.push_back(10);
    vv.push_back(20.0f);
    vv.push_back(30);

    const std::array<std::any, 3> expect{std::any{10}, std::any{20.0f}, std::any{30}};
    size_t i{0};

    for (const auto &val : vv) {
        std::visit([&expect, &i](auto &&val){
            ASSERT_EQ(val, std::any_cast<decltype(val)>(expect[i++]));
        }, val);
    }
}


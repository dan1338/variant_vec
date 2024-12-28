enable_testing()

add_executable(test_variant_vec
    test_variant_vec.cpp)

target_link_libraries(test_variant_vec GTest::gtest_main variant_vec)

include(GoogleTest)
gtest_discover_tests(test_variant_vec)


#include <gtest/gtest.h>
#include <string>

#include "pmr_dynamic_array.h"
#include "vector_tracking_resource.h"

namespace {

struct animal {
    std::string name;
    int age{};
    double weight{};
};

}  // namespace

TEST(PmrDynamicArrayTest, HandlesInts) {
    hw5::vector_tracking_resource resource;
    hw5::pmr_dynamic_array<int> ints{std::pmr::polymorphic_allocator<int>(&resource)};

    for (int i = 0; i < 5; ++i) {
        ints.push_back(i);
    }

    ASSERT_EQ(ints.size(), 5);
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(ints[i], i);
    }
}

TEST(PmrDynamicArrayTest, HandlesCustomStruct) {
    hw5::vector_tracking_resource resource;
    hw5::pmr_dynamic_array<animal> animals{std::pmr::polymorphic_allocator<animal>(&resource)};

    animals.emplace_back(animal{"Fox", 4, 8.1});
    animals.emplace_back(animal{"Bear", 7, 120.0});

    ASSERT_EQ(animals.size(), 2);
    EXPECT_DOUBLE_EQ(animals[1].weight, 120.0);

    animals.pop_back();
    animals.emplace_back(animal{"Wolf", 5, 35.0});

    ASSERT_EQ(animals.size(), 2);
    EXPECT_EQ(animals[1].name, "Wolf");
}


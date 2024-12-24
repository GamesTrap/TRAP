#include <catch2/catch_test_macros.hpp>

#include "TRAP_Assert.h"

#include "Utils/ConstexprMap.h"

TEST_CASE("TRAP::Utils::ConstexprMap", "[utils][constexprmap]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Utils::ConstexprMap<std::string_view, std::string_view, 1u>>);
        STATIC_REQUIRE(std::copyable<TRAP::Utils::ConstexprMap<std::string_view, std::string_view, 1u>>);
        STATIC_REQUIRE(std::movable<TRAP::Utils::ConstexprMap<std::string_view, std::string_view, 1u>>);

        using ConstexprMapTest = TRAP::Utils::ConstexprMap<i32, std::string_view, 10u>;
        STATIC_REQUIRE(std::same_as<ConstexprMapTest::key_type, i32>);
        STATIC_REQUIRE(std::same_as<ConstexprMapTest::mapped_type, std::string_view>);
        STATIC_REQUIRE(std::same_as<ConstexprMapTest::value_type, std::pair<i32, std::string_view>>);
        STATIC_REQUIRE(std::same_as<ConstexprMapTest::size_type, usize>);
        STATIC_REQUIRE(std::same_as<ConstexprMapTest::difference_type, isize>);
        STATIC_REQUIRE(std::same_as<ConstexprMapTest::key_equal, std::equal_to<i32>>);
        STATIC_REQUIRE(std::same_as<ConstexprMapTest::reference, std::pair<i32, std::string_view>&>);
        STATIC_REQUIRE(std::same_as<ConstexprMapTest::const_reference, const std::pair<i32, std::string_view>&>);
        STATIC_REQUIRE(std::same_as<ConstexprMapTest::pointer, std::pair<i32, std::string_view>*>);
        STATIC_REQUIRE(std::same_as<ConstexprMapTest::const_pointer, const std::pair<i32, std::string_view>*>);
        STATIC_REQUIRE(std::same_as<ConstexprMapTest::iterator, std::array<std::pair<i32, std::string_view>, 10u>::iterator>);
        STATIC_REQUIRE(std::same_as<ConstexprMapTest::const_iterator, std::array<std::pair<i32, std::string_view>, 10u>::const_iterator>);
    }

    SECTION("Constructors")
    {
        static constexpr TRAP::Utils::ConstexprMap<std::string_view, std::string_view, 1u> map{};

        //Copy constructor
        static constexpr TRAP::Utils::ConstexprMap<std::string_view, std::string_view, 1u> mapCopy(map);

        //Move constructor
        TRAP::Utils::ConstexprMap<std::string_view, std::string_view, 1u> mapMoveSource{};
        const TRAP::Utils::ConstexprMap<std::string_view, std::string_view, 1u> mapMoveTarget(std::move(mapMoveSource));

        static constexpr std::array<std::pair<i32, std::string_view>, 2u> arr
        {
            {
                {0, "Hello"},
                {1, "World"}
            }
        };
        //Array constructor
        static constexpr TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> arrMap{arr};

        //Iterator constructor
        static constexpr TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> arrMap2(arr.begin(), arr.end());
        TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> arrMap3(arr.begin(), arr.end());

        //Initializer list constructor
        static constexpr TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapInitializerList
        {
            {
                {0, "Hello"},
                {1, "World"}
            }
        };
        TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapInitializerList2
        {
            {
                {0, "Hello"},
                {1, "World"}
            }
        };
        //Invalid
        TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapInitializerListInvalid
        {
            {
                {0, "Hello"},
                {1, "World"},
                {2, "hehe"}
            }
        };
        TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapInitializerListInvalid2
        {
            {
                {0, "Hello"}
            }
        };
    }

    SECTION("Assignments")
    {
        static constexpr TRAP::Utils::ConstexprMap<std::string_view, std::string_view, 1u> map{};

        //Copy assignment operator
        static constexpr TRAP::Utils::ConstexprMap<std::string_view, std::string_view, 1u> mapCopy = map;

        //Move assignment operator
        TRAP::Utils::ConstexprMap<std::string_view, std::string_view, 1u> mapMoveSource{};
        const TRAP::Utils::ConstexprMap<std::string_view, std::string_view, 1u> mapMoveTarget = std::move(mapMoveSource);

        //Initializer list
        static constexpr TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapInitializerList = std::initializer_list<std::pair<i32, std::string_view>>
        {
            {0, "Hello"},
            {1, "World"}
        };
        TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapInitializerList2 = std::initializer_list<std::pair<i32, std::string_view>>
        {
            {0, "Hello"},
            {1, "World"}
        };
        //Invalid
        TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapInitializerListInvalid = std::initializer_list<std::pair<i32, std::string_view>>
        {
            {0, "Hello"},
            {1, "World"},
            {2, "hehe"}
        };
        TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapInitializerListInvalid2 = std::initializer_list<std::pair<i32, std::string_view>>
        {
            {0, "Hello"}
        };
    }

    SECTION("begin()")
    {
        static constexpr TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapConstexpr
        {
            {
                {
                    {0, "Hello"},
                    {1, "World"}
                }
            }
        };

        STATIC_REQUIRE(std::same_as<decltype(mapConstexpr.begin()), std::array<std::pair<i32, std::string_view>, 2u>::const_iterator>);
        STATIC_REQUIRE(mapConstexpr.begin() != mapConstexpr.end());

        TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> map
        {
            {
                {
                    {0, "Hello"},
                    {1, "World"}
                }
            }
        };

        STATIC_REQUIRE(std::same_as<decltype(map.begin()), std::array<std::pair<i32, std::string_view>, 2u>::iterator>);
        REQUIRE(map.begin() != map.end());
    }

    SECTION("cbegin()")
    {
        static constexpr TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapConstexpr
        {
            {
                {
                    {0, "Hello"},
                    {1, "World"}
                }
            }
        };

        STATIC_REQUIRE(std::same_as<decltype(mapConstexpr.cbegin()), std::array<std::pair<i32, std::string_view>, 2u>::const_iterator>);
        STATIC_REQUIRE(mapConstexpr.cbegin() != mapConstexpr.end());
    }

    SECTION("end()")
    {
        static constexpr TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapConstexpr
        {
            {
                {
                    {0, "Hello"},
                    {1, "World"}
                }
            }
        };

        STATIC_REQUIRE(std::same_as<decltype(mapConstexpr.end()), std::array<std::pair<i32, std::string_view>, 2u>::const_iterator>);
        STATIC_REQUIRE(mapConstexpr.end() == mapConstexpr.end());

        TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> map
        {
            {
                {
                    {0, "Hello"},
                    {1, "World"}
                }
            }
        };

        STATIC_REQUIRE(std::same_as<decltype(map.end()), std::array<std::pair<i32, std::string_view>, 2u>::iterator>);
        REQUIRE(map.end() == map.end());
    }

    SECTION("cend()")
    {
        static constexpr TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapConstexpr
        {
            {
                {
                    {0, "Hello"},
                    {1, "World"}
                }
            }
        };

        STATIC_REQUIRE(std::same_as<decltype(mapConstexpr.cend()), std::array<std::pair<i32, std::string_view>, 2u>::const_iterator>);
        STATIC_REQUIRE(mapConstexpr.cend() == mapConstexpr.cend());
    }

    SECTION("at()")
    {
        static constexpr TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapConstexpr
        {
            {
                {
                    {0, "Hello"},
                    {1, "World"}
                }
            }
        };

        STATIC_REQUIRE_FALSE(mapConstexpr.at(128));
        STATIC_REQUIRE_FALSE(mapConstexpr.at<i64>(128));

        STATIC_REQUIRE(mapConstexpr.at(0));
        STATIC_REQUIRE(*mapConstexpr.at(0) == "Hello");
        STATIC_REQUIRE(mapConstexpr.at(1));
        STATIC_REQUIRE(*mapConstexpr.at(1) == "World");

        STATIC_REQUIRE(mapConstexpr.at<i64>(0));
        STATIC_REQUIRE(*mapConstexpr.at<i64>(0) == "Hello");
        STATIC_REQUIRE(mapConstexpr.at<i64>(1));
        STATIC_REQUIRE(*mapConstexpr.at<i64>(1) == "World");
    }

    SECTION("find()")
    {
        static constexpr TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapConstexpr
        {
            {
                {
                    {0, "Hello"},
                    {1, "World"}
                }
            }
        };

        STATIC_REQUIRE(mapConstexpr.find(128) == mapConstexpr.end());
        STATIC_REQUIRE(mapConstexpr.find<i64>(128) == mapConstexpr.end());

        STATIC_REQUIRE(mapConstexpr.find(0) != mapConstexpr.end());
        STATIC_REQUIRE(mapConstexpr.find(0) != mapConstexpr.end());

        TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapConstexpr2
        {
            {
                {
                    {0, "Hello"},
                    {1, "World"}
                }
            }
        };

        REQUIRE(mapConstexpr2.find(128) == mapConstexpr2.end());
        REQUIRE(mapConstexpr2.find<i64>(128) == mapConstexpr2.end());

        REQUIRE(mapConstexpr2.find(0) != mapConstexpr2.end());
        REQUIRE(mapConstexpr2.find(0) != mapConstexpr2.end());
    }

    SECTION("contains()")
    {
        static constexpr TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapConstexpr
        {
            {
                {
                    {0, "Hello"},
                    {1, "World"}
                }
            }
        };

        STATIC_REQUIRE_FALSE(mapConstexpr.contains(128));
        STATIC_REQUIRE(mapConstexpr.contains(0));
        STATIC_REQUIRE(mapConstexpr.contains(1));
    }

    SECTION("empty()")
    {
        static constexpr TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapConstexpr
        {
            {
                {
                    {0, "Hello"},
                    {1, "World"}
                }
            }
        };

        STATIC_REQUIRE_FALSE(mapConstexpr.empty());

        STATIC_REQUIRE(TRAP::Utils::ConstexprMap<i32, std::string_view, 0u>{}.empty());
    }

    SECTION("size()")
    {
        static constexpr TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapConstexpr
        {
            {
                {
                    {0, "Hello"},
                    {1, "World"}
                }
            }
        };

        STATIC_REQUIRE(mapConstexpr.size() == 2);

        STATIC_REQUIRE(TRAP::Utils::ConstexprMap<i32, std::string_view, 0u>{}.size() == 0);
    }

    SECTION("operator[]()")
    {
        static constexpr TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapConstexpr
        {
            {
                {
                    {0, "Hello"},
                    {1, "World"}
                }
            }
        };

        const i32 idx = 0;
        STATIC_REQUIRE(mapConstexpr[idx] == "Hello");
        const i32 idxInvalid = 128;
        REQUIRE_THROWS_AS(mapConstexpr[idxInvalid], std::bad_optional_access);

        STATIC_REQUIRE(mapConstexpr[0] == "Hello");
        STATIC_REQUIRE(mapConstexpr[1] == "World");
        STATIC_REQUIRE(mapConstexpr[u64(0u)] == "Hello");
        STATIC_REQUIRE(mapConstexpr[u64(1u)] == "World");
        REQUIRE_THROWS_AS(mapConstexpr[128], std::bad_optional_access);
    }

    SECTION("count()")
    {
        static constexpr TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapConstexpr
        {
            {
                {
                    {0, "Hello"},
                    {1, "World"}
                }
            }
        };

        STATIC_REQUIRE(mapConstexpr.count(0u) == 1);
        STATIC_REQUIRE(mapConstexpr.count(1u) == 1);
        STATIC_REQUIRE(mapConstexpr.count(128u) == 0);
    }

    SECTION("operator==()")
    {
        static constexpr TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapConstexpr
        {
            {
                {
                    {0, "Hello"},
                    {1, "World"}
                }
            }
        };

        STATIC_REQUIRE(mapConstexpr == mapConstexpr);
        STATIC_REQUIRE_FALSE(mapConstexpr == TRAP::Utils::ConstexprMap<i32, std::string_view, 2u>{});
    }

    SECTION("operator!=()")
    {
        static constexpr TRAP::Utils::ConstexprMap<i32, std::string_view, 2u> mapConstexpr
        {
            {
                {
                    {0, "Hello"},
                    {1, "World"}
                }
            }
        };

        STATIC_REQUIRE_FALSE(mapConstexpr != mapConstexpr);
        STATIC_REQUIRE(mapConstexpr != TRAP::Utils::ConstexprMap<i32, std::string_view, 2u>{});
    }
}

//
// Includes
//
#include <catch2/catch_test_macros.hpp>

#include "uni_common_time.h"


//
// tests
//

TEST_CASE("time_unix2tm", "[time]") {
    time_t ut = 1641904524UL;
    tm tm{};

    REQUIRE(uni_common_time_unix_to_tm(&ut, &tm));
    REQUIRE(tm.tm_year == 2022 - 1900); // 2022
    REQUIRE(tm.tm_mon == 0);            // january
    REQUIRE(tm.tm_mday == 11);          // 11

    REQUIRE(tm.tm_yday == 10); // 11th day since year start
    REQUIRE(tm.tm_wday == 2);  // wednesday

    REQUIRE(tm.tm_isdst == 0);

    REQUIRE(tm.tm_hour == 12);
    REQUIRE(tm.tm_min == 35);
    REQUIRE(tm.tm_sec == 24);
}


TEST_CASE("time_tm2unix", "[time]") {
    time_t ut = 0;
    tm tm{};

    tm.tm_year = 2022 - 1900; // 2022
    tm.tm_mon = 0;            // january
    tm.tm_mday = 11;          // 11
    tm.tm_yday = 10;          // 11th day since year start
    tm.tm_wday = 2;           // wednesday
    tm.tm_isdst = 0;
    tm.tm_hour = 12;
    tm.tm_min = 35;
    tm.tm_sec = 24;

    REQUIRE(uni_common_time_tm_to_unix(&tm, &ut));
    REQUIRE(ut == 1641904524UL);
}


TEST_CASE("time_unpack", "[time]") {
    REQUIRE_FALSE(uni_common_time_unpack(NULL, NULL, NULL));

    time_t in = 0x0102030405060708LL;
    uint32_t out_h = 0;
    uint32_t out_l = 0;

    REQUIRE(uni_common_time_unpack(&in, &out_h, &out_l));
    REQUIRE(out_h == 0x01020304);
    REQUIRE(out_l == 0x05060708);
}

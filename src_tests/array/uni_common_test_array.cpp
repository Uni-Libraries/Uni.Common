//
// Includes
//

#include <cstring>

#include <catch2/catch_test_macros.hpp>

#include "uni_common.h"


//
// tests
//

TEST_CASE("array_init", "[array]") {
    uint8_t buf[16]{};
    unimcu_array_t ctx{};

    for (int p1 = 0; p1 <= 1; p1++) {
        for (int p2 = 0; p2 <= 1; p2++) {
            for (int p3 = 0; p3 <= 1; p3++) {
                for (int p4 = 0; p4 <= 1; p4++) {

                    if (p1 && p2 && p3 && p4) {
                        continue;
                    }

                    unimcu_array_t *ctx_p = p1 ? &ctx : nullptr;
                    uint8_t *arr_p = p2 ? buf : nullptr;
                    size_t buf_size = p3 ? sizeof(buf) : 0;
                    size_t el_size = p4 ? sizeof(buf[0]) : 0;

                    REQUIRE_FALSE(unimcu_array_init(ctx_p, arr_p, buf_size, el_size));
                    REQUIRE_FALSE(unimcu_array_valid(ctx_p));
                }
            }
        }
    }

    REQUIRE(unimcu_array_init(&ctx, buf, sizeof(buf), 1));
    REQUIRE(unimcu_array_valid(&ctx));
}

TEST_CASE("array_itemsize", "[array]") {
    SECTION("get") {
        uint8_t buf[16]{};
        unimcu_array_t ctx{};

        REQUIRE(unimcu_array_itemsize(nullptr) == 0);
        REQUIRE(unimcu_array_length(nullptr) == 0);
        REQUIRE(unimcu_array_size(nullptr) == 0);

        REQUIRE(unimcu_array_init(&ctx, buf, sizeof(buf), 1));
        REQUIRE(unimcu_array_valid(&ctx));

        REQUIRE(unimcu_array_itemsize(&ctx) == 1);
        REQUIRE(unimcu_array_length(&ctx) == 16);
        REQUIRE(unimcu_array_size(&ctx) == 16);

        ctx.size_item = 2;
        REQUIRE(unimcu_array_itemsize(&ctx) == 2);
        REQUIRE(unimcu_array_length(&ctx) == 8);
        REQUIRE(unimcu_array_size(&ctx) == 16);
    }

    SECTION("set") {
        REQUIRE_FALSE(unimcu_array_set_itemsize(nullptr, 5));

        uint8_t buf[16]{};
        unimcu_array_t ctx{};
        REQUIRE(unimcu_array_init(&ctx, buf, sizeof(buf), 1));

        REQUIRE(unimcu_array_set_itemsize(&ctx, 5));
        REQUIRE(unimcu_array_itemsize(&ctx) == 5);

        REQUIRE(unimcu_array_set_itemsize(&ctx, sizeof(buf)));
        REQUIRE(unimcu_array_itemsize(&ctx) == sizeof(buf));

        REQUIRE_FALSE(unimcu_array_set_itemsize(&ctx, sizeof(buf) + 1));
        REQUIRE(unimcu_array_itemsize(&ctx) == sizeof(buf));
    }
}


TEST_CASE("array_pack", "[array]") {
    uint8_t buf_1[3]{};
    uint8_t buf_2[6]{};

    uint8_t buf_3[9]{};
    uint8_t buf_4[10]{};
    uint8_t buf_5[8]{};

    unimcu_array_t ctx[2]{};

    REQUIRE(unimcu_array_init(&ctx[0], buf_1, sizeof(buf_1), 1));
    REQUIRE(unimcu_array_init(&ctx[1], buf_2, sizeof(buf_2), 1));

    for (size_t i = 0; i < sizeof(buf_1); i++) {
        buf_1[i] = i;
    }
    for (size_t i = 0; i < sizeof(buf_2); i++) {
        buf_2[i] = i;
    }

    REQUIRE(unimcu_array_pack(buf_3, sizeof(buf_3), ctx, 2));
    REQUIRE(memcmp(buf_3, buf_1, sizeof(buf_1)) == 0);
    REQUIRE(memcmp(buf_3 + sizeof(buf_1), buf_2, sizeof(buf_2)) == 0);

    REQUIRE(unimcu_array_pack(buf_4, sizeof(buf_4), ctx, 2));
    REQUIRE(memcmp(buf_4, buf_1, sizeof(buf_1)) == 0);
    REQUIRE(memcmp(buf_4 + sizeof(buf_1), buf_2, sizeof(buf_2)) == 0);

    REQUIRE_FALSE(unimcu_array_pack(buf_5, sizeof(buf_5), ctx, 2));
}

TEST_CASE("array_getset", "[array]") {
    uint8_t buf[16]{};
    unimcu_array_t ctx{};
    REQUIRE(unimcu_array_init(&ctx, buf, sizeof(buf), 1));

    for (size_t i = 0; i < sizeof(buf); i++) {
        buf[i] = i;
    }

    SECTION("get") {
        for (size_t i = 0; i < sizeof(buf); i++) {
            REQUIRE(&buf[i] == unimcu_array_get(&ctx, i));
            REQUIRE(memcmp(&buf[i], unimcu_array_get(&ctx, i), 1) == 0);
        }

        ctx.size_item = 2;

        for (size_t i = 0, j = 0; i < sizeof(buf); i += 2, j += 1) {
            REQUIRE(&buf[i] == unimcu_array_get(&ctx, j));
            REQUIRE(memcmp(&buf[i], unimcu_array_get(&ctx, j), 1) == 0);
        }

        ctx.size_item = 1;
    }

    SECTION("set") {
        for (size_t i = 0; i < sizeof(buf); i++) {
            uint8_t num = i * 2;
            unimcu_array_set(&ctx, i, &num);
            REQUIRE(&buf[i] == unimcu_array_get(&ctx, i));
            REQUIRE(memcmp(&buf[i], unimcu_array_get(&ctx, i), 1) == 0);
            REQUIRE(memcmp(&num, unimcu_array_get(&ctx, i), 1) == 0);
        }

        ctx.size_item = 2;

        for (size_t i = 0, j = 0; i < sizeof(buf); i += 2, j += 1) {
            uint8_t num = i * 2;
            unimcu_array_set(&ctx, j, &num);
            REQUIRE(&buf[i] == unimcu_array_get(&ctx, j));
            REQUIRE(memcmp(&buf[i], unimcu_array_get(&ctx, j), 1) == 0);
            REQUIRE(memcmp(&num, unimcu_array_get(&ctx, j), 1) == 0);
        }

        ctx.size_item = 1;
    }
}

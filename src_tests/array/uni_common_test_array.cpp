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
    uni_common_array_t ctx{};

    for (int p1 = 0; p1 <= 1; p1++) {
        for (int p2 = 0; p2 <= 1; p2++) {
            for (int p3 = 0; p3 <= 1; p3++) {
                for (int p4 = 0; p4 <= 1; p4++) {

                    if (p1 && p2 && p3 && p4) {
                        continue;
                    }

                    uni_common_array_t *ctx_p = p1 ? &ctx : nullptr;
                    uint8_t *arr_p = p2 ? buf : nullptr;
                    size_t buf_size = p3 ? sizeof(buf) : 0;
                    size_t el_size = p4 ? sizeof(buf[0]) : 0;

                    REQUIRE_FALSE(uni_common_array_init(ctx_p, arr_p, buf_size, el_size));
                    REQUIRE_FALSE(uni_common_array_valid(ctx_p));
                }
            }
        }
    }

    REQUIRE(uni_common_array_init(&ctx, buf, sizeof(buf), 1));
    REQUIRE(uni_common_array_valid(&ctx));
}

TEST_CASE("array_itemsize", "[array]") {
    SECTION("get") {
        uint8_t buf[16]{};
        uni_common_array_t ctx{};

        REQUIRE(uni_common_array_itemsize(nullptr) == 0);
        REQUIRE(uni_common_array_size(nullptr) == 0);
        REQUIRE(uni_common_array_size_bytes(nullptr) == 0);

        REQUIRE(uni_common_array_init(&ctx, buf, sizeof(buf), 1));
        REQUIRE(uni_common_array_valid(&ctx));

        REQUIRE(uni_common_array_itemsize(&ctx) == 1);
        REQUIRE(uni_common_array_size(&ctx) == 16);
        REQUIRE(uni_common_array_size_bytes(&ctx) == 16);

        ctx.size_item = 2;
        REQUIRE(uni_common_array_itemsize(&ctx) == 2);
        REQUIRE(uni_common_array_size(&ctx) == 8);
        REQUIRE(uni_common_array_size_bytes(&ctx) == 16);
    }

    SECTION("set") {
        REQUIRE_FALSE(uni_common_array_set_itemsize(nullptr, 5));

        uint8_t buf[16]{};
        uni_common_array_t ctx{};
        REQUIRE(uni_common_array_init(&ctx, buf, sizeof(buf), 1));

        REQUIRE(uni_common_array_set_itemsize(&ctx, 5));
        REQUIRE(uni_common_array_itemsize(&ctx) == 5);

        REQUIRE(uni_common_array_set_itemsize(&ctx, sizeof(buf)));
        REQUIRE(uni_common_array_itemsize(&ctx) == sizeof(buf));

        REQUIRE_FALSE(uni_common_array_set_itemsize(&ctx, sizeof(buf) + 1));
        REQUIRE(uni_common_array_itemsize(&ctx) == sizeof(buf));
    }
}


TEST_CASE("array_getset", "[array]") {
    uint8_t buf[16]{};
    uni_common_array_t ctx{};
    REQUIRE(uni_common_array_init(&ctx, buf, sizeof(buf), 1));

    for (size_t i = 0; i < sizeof(buf); i++) {
        buf[i] = i;
    }

    SECTION("get") {
        for (size_t i = 0; i < sizeof(buf); i++) {
            REQUIRE(&buf[i] == uni_common_array_get(&ctx, i));
            REQUIRE(memcmp(&buf[i], uni_common_array_get(&ctx, i), 1) == 0);
        }

        ctx.size_item = 2;

        for (size_t i = 0, j = 0; i < sizeof(buf); i += 2, j += 1) {
            REQUIRE(&buf[i] == uni_common_array_get(&ctx, j));
            REQUIRE(memcmp(&buf[i], uni_common_array_get(&ctx, j), 1) == 0);
        }

        ctx.size_item = 1;
    }

    SECTION("set") {
        for (size_t i = 0; i < sizeof(buf); i++) {
            uint8_t num = i * 2;
            uni_common_array_set(&ctx, i, &num);
            REQUIRE(&buf[i] == uni_common_array_get(&ctx, i));
            REQUIRE(memcmp(&buf[i], uni_common_array_get(&ctx, i), 1) == 0);
            REQUIRE(memcmp(&num, uni_common_array_get(&ctx, i), 1) == 0);
        }

        ctx.size_item = 2;

        for (size_t i = 0, j = 0; i < sizeof(buf); i += 2, j += 1) {
            uint8_t num = i * 2;
            uni_common_array_set(&ctx, j, &num);
            REQUIRE(&buf[i] == uni_common_array_get(&ctx, j));
            REQUIRE(memcmp(&buf[i], uni_common_array_get(&ctx, j), 1) == 0);
            REQUIRE(memcmp(&num, uni_common_array_get(&ctx, j), 1) == 0);
        }

        ctx.size_item = 1;
    }
}

TEST_CASE("array_frontback", "[array]") {
    uint8_t buf[16]{};
    uni_common_array_t ctx{};
    REQUIRE(uni_common_array_init(&ctx, buf, sizeof(buf), 1));
    REQUIRE(uni_common_array_clear(&ctx));
    REQUIRE(uni_common_array_valid(&ctx));

    REQUIRE(uni_common_array_front(&ctx) == nullptr);
    REQUIRE(uni_common_array_back(&ctx) == nullptr);

    uint8_t val1 = 1;
    REQUIRE(uni_common_array_push_back(&ctx, &val1));
    REQUIRE(uni_common_array_size(&ctx) == 1);
    REQUIRE(*(uint8_t*)uni_common_array_front(&ctx) == val1);
    REQUIRE(*(uint8_t*)uni_common_array_back(&ctx) == val1);

    uint8_t val2 = 2;
    REQUIRE(uni_common_array_push_back(&ctx, &val2));
    REQUIRE(uni_common_array_size(&ctx) == 2);
    REQUIRE(*(uint8_t*)uni_common_array_front(&ctx) == val1);
    REQUIRE(*(uint8_t*)uni_common_array_back(&ctx) == val2);
}

TEST_CASE("array_reserve", "[array]") {
    uint8_t buf[16]{};
    uni_common_array_t ctx{};
    REQUIRE(uni_common_array_init(&ctx, buf, sizeof(buf), 1));
    REQUIRE(uni_common_array_clear(&ctx));
    REQUIRE(uni_common_array_valid(&ctx));

    REQUIRE(uni_common_array_reserve(&ctx, 10));
    REQUIRE(uni_common_array_size(&ctx) == 10);
    REQUIRE_FALSE(uni_common_array_reserve(&ctx, 20));
    REQUIRE(uni_common_array_size(&ctx) == 10);
}

TEST_CASE("array_dynamic", "[array]") {
    uni_common_array_t *ctx = uni_common_array_create(16, 1);

    REQUIRE(uni_common_array_valid(ctx));
    REQUIRE(uni_common_array_capacity(ctx) == 16);
    REQUIRE(uni_common_array_size(ctx) == 16);
    REQUIRE(uni_common_array_itemsize(ctx) == 1);

    REQUIRE(uni_common_array_free(ctx));
}


UNI_COMMON_ARRAY_DEFINITION_EX(g_array, uint32_t, 16);

TEST_CASE("array_macro", "[array]") {
    REQUIRE(uni_common_array_valid(&g_array_ctx));
    REQUIRE(uni_common_array_capacity(&g_array_ctx) == 16);
    REQUIRE(uni_common_array_size(&g_array_ctx) == 16);
    REQUIRE(uni_common_array_itemsize(&g_array_ctx) == 4);
}

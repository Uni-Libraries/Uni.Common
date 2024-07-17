//
// Includes
//

#include <cstring>

#include <catch2/catch_test_macros.hpp>

#include "uni_common.h"


//
// Static
//

static uint32_t _temp = 0;
static constexpr size_t _capacity = 32;
static unimcu_map_context_t _ctx;

static unimcu_array_t _arr_keys{};
static size_t _arr_keys_buf[_capacity];

static unimcu_array_t _arr_vals{};
static size_t _arr_vals_buf[_capacity];


//
// Private
//

bool _map_init() {
    memset(&_ctx, 0, sizeof(_ctx));

    memset(_arr_keys_buf, 0, sizeof(_arr_keys_buf));
    memset(_arr_vals_buf, 0, sizeof(_arr_vals_buf));

    memset(&_arr_keys, 0, sizeof(_arr_keys));
    memset(&_arr_vals, 0, sizeof(_arr_vals));

    REQUIRE_FALSE(unimcu_map_initialized(&_ctx));
    unimcu_array_init(&_arr_keys, (uint8_t *)_arr_keys_buf, sizeof(_arr_keys_buf), sizeof(size_t));
    unimcu_array_init(&_arr_vals, (uint8_t *)_arr_vals_buf, sizeof(_arr_vals_buf), sizeof(size_t));

    bool result = unimcu_map_init(&_ctx, &_arr_keys, &_arr_vals);

    REQUIRE(unimcu_map_initialized(&_ctx));

    return result;
}


//
// Tests
//

TEST_CASE("map_init", "[map]") {
    SECTION("nullptr") { REQUIRE_FALSE(unimcu_map_init(nullptr, nullptr, nullptr)); }

    SECTION("helper") {
        REQUIRE(_map_init());
        REQUIRE(_map_init());
        REQUIRE(unimcu_map_size(&_ctx) == 0);
    }
}


TEST_CASE("map_set", "[map]") {
    SECTION("nullptr") { REQUIRE_FALSE(unimcu_map_set(nullptr, 0, nullptr)); }

    SECTION("ok") {
        _map_init();
        REQUIRE(unimcu_map_size(&_ctx) == 0);

        size_t val = 0;
        REQUIRE(unimcu_map_set(&_ctx, 0, &val));
        REQUIRE(unimcu_map_size(&_ctx) == 1);

        REQUIRE(unimcu_map_set(&_ctx, 1, &val));
        REQUIRE(unimcu_map_size(&_ctx) == 2);

        REQUIRE(unimcu_map_set(&_ctx, 1, &val));
        REQUIRE(unimcu_map_size(&_ctx) == 2);
    }

    SECTION("ok-2"){
        //init
        _map_init();
        REQUIRE(unimcu_map_size(&_ctx) == 0);

        //add 1-->2
        size_t key = 1;
        size_t val = 2;
        REQUIRE(unimcu_map_set(&_ctx, key, &val));
        REQUIRE(*(size_t*)unimcu_map_get(&_ctx, key) == val);
        REQUIRE(unimcu_map_size(&_ctx) == 1);


        //add 2->3
        key = 2;
        val = 3;
        REQUIRE(unimcu_map_set(&_ctx, key, &val));
        REQUIRE(*(size_t*)unimcu_map_get(&_ctx, key) == val);
        REQUIRE(unimcu_map_size(&_ctx) == 2);

        //add 1->4
        key = 1;
        val = 4;
        REQUIRE(unimcu_map_set(&_ctx, key, &val));
        REQUIRE(*(size_t*)unimcu_map_get(&_ctx, key) == val);
        REQUIRE(unimcu_map_size(&_ctx) == 2);
    }

    SECTION("ok-3") {
        _map_init();

        for (size_t idx = 0; idx < unimcu_map_capacity(&_ctx); idx++) {
            REQUIRE(unimcu_map_set(&_ctx, idx, &idx));
        }

        size_t val = 100;
        REQUIRE_FALSE(unimcu_map_set(&_ctx, val, &val));
    }
}


TEST_CASE("map_get", "[map]") {
    SECTION("nullptr") { REQUIRE(unimcu_map_get(nullptr, 0) == nullptr); }

    SECTION("ok") {
        _map_init();
        REQUIRE(unimcu_map_size(&_ctx) == 0);
        REQUIRE(unimcu_map_get(&_ctx, 0) == nullptr);

        size_t val = 1;
        REQUIRE(unimcu_map_set(&_ctx, val - 1, &val));
        REQUIRE(*(size_t *)unimcu_map_get(&_ctx, 0) == 1);

        val = 2;
        REQUIRE(unimcu_map_set(&_ctx, val - 1, &val));
        REQUIRE(*(size_t *)unimcu_map_get(&_ctx, val - 1) == val);

        val = 2;
        REQUIRE(unimcu_map_set(&_ctx, val - 2, &val));
        REQUIRE(*(size_t *)unimcu_map_get(&_ctx, val - 2) == val);
    }
}


TEST_CASE("map_remove", "[map]") {
    SECTION("nullptr") { REQUIRE_FALSE(unimcu_map_remove(nullptr, 0)); }

    SECTION("ok") {
        //init
        _map_init();
        REQUIRE(unimcu_map_size(&_ctx) == 0);

        size_t val = 0;
        REQUIRE(unimcu_map_set(&_ctx, 0, &val));
        REQUIRE(unimcu_map_set(&_ctx, 1, &val));
        REQUIRE(unimcu_map_set(&_ctx, 2, &val));


        REQUIRE(unimcu_map_size(&_ctx) == 3);

        REQUIRE_FALSE(unimcu_map_remove(&_ctx, 4));
        REQUIRE(unimcu_map_size(&_ctx) == 3);

        REQUIRE(unimcu_map_remove(&_ctx, 2));
        REQUIRE(unimcu_map_size(&_ctx) == 2);

        REQUIRE_FALSE(unimcu_map_remove(&_ctx, 2));
        REQUIRE(unimcu_map_size(&_ctx) == 2);
    }

    SECTION("ok-first-1"){
        //init
        _map_init();
        REQUIRE(unimcu_map_size(&_ctx) == 0);

        //create 1->0, 2->0
        size_t val = 0;
        REQUIRE(unimcu_map_set(&_ctx, 1, &val));
        REQUIRE(unimcu_map_set(&_ctx, 2, &val));
        REQUIRE(unimcu_map_size(&_ctx) == 2);

        //remove 1->0
        REQUIRE(unimcu_map_remove(&_ctx, 1));
        REQUIRE(unimcu_map_size(&_ctx) == 1);

        // remove 2->0
        REQUIRE(unimcu_map_remove(&_ctx, 2));
        REQUIRE(unimcu_map_size(&_ctx) == 0);
    }

    SECTION("ok-first-2") {
        // init
        _map_init();
        REQUIRE(unimcu_map_size(&_ctx) == 0);

        // create 1->0, 2->0
        size_t val = 0;
        REQUIRE(unimcu_map_set(&_ctx, 1, &val));
        REQUIRE(unimcu_map_set(&_ctx, 2, &val));
        REQUIRE(unimcu_map_size(&_ctx) == 2);
    }

    SECTION("ok-last-1") {
        // init
        _map_init();
        REQUIRE(unimcu_map_size(&_ctx) == 0);

        // create 1->0, 2->0
        size_t val = 0;
        REQUIRE(unimcu_map_set(&_ctx, 1, &val));
        REQUIRE(unimcu_map_set(&_ctx, 2, &val));
        REQUIRE(unimcu_map_size(&_ctx) == 2);

        //remove 2->0
        REQUIRE(unimcu_map_remove(&_ctx, 2));
        REQUIRE(unimcu_map_size(&_ctx) == 1);

        // remove 1->0
        REQUIRE(unimcu_map_remove(&_ctx, 1));
        REQUIRE(unimcu_map_size(&_ctx) == 0);
    }

    SECTION("ok-last-2") {
        // init
        _map_init();
        REQUIRE(unimcu_map_size(&_ctx) == 0);

        // create 1->0, 2->0
        size_t val = 0;
        REQUIRE(unimcu_map_set(&_ctx, 1, &val));
        REQUIRE(unimcu_map_set(&_ctx, 2, &val));
        REQUIRE(unimcu_map_size(&_ctx) == 2);
    }
}


TEST_CASE("map_size", "[map]") {
    SECTION("nullptr") { REQUIRE_FALSE(unimcu_map_size(nullptr)); }

    SECTION("ok") {
        _map_init();
        REQUIRE(unimcu_map_size(&_ctx) == 0);

        size_t val = 0;
        REQUIRE(unimcu_map_set(&_ctx, 0, &val));
        REQUIRE(unimcu_map_size(&_ctx) == 1);

        REQUIRE(unimcu_map_remove(&_ctx, 0));
        REQUIRE(unimcu_map_size(&_ctx) == 0);
    }
}


TEST_CASE("map_enum", "[map]") {
    SECTION("nullptr_obj") { REQUIRE_FALSE(unimcu_map_enum(nullptr, nullptr)); }

    SECTION("nullptr_func") {
        _map_init();
        REQUIRE_FALSE(unimcu_map_enum(&_ctx, nullptr));
    }

    SECTION("ok") {
        _map_init();

        size_t capacity = unimcu_map_capacity(&_ctx);
        for (size_t idx = 1; idx <= capacity; idx++) {
            REQUIRE(unimcu_map_set(&_ctx, idx - 1, &idx));
        }

        _temp = 0;
        REQUIRE(unimcu_map_enum(&_ctx, [](size_t key, const void *val) {
            REQUIRE((key == _temp));
            REQUIRE(((*(const uint32_t *)val) == _temp + 1));
            _temp++;
        }));
    }
}


TEST_CASE("map_clear", "[map]") {
    SECTION("nullptr") { REQUIRE_FALSE(unimcu_map_clear(nullptr)); }

    SECTION("ok") {
        _map_init();

        size_t val = 0;
        REQUIRE(unimcu_map_set(&_ctx, 0, &val));
        REQUIRE(unimcu_map_size(&_ctx) == 1);

        REQUIRE(unimcu_map_clear(&_ctx));
        REQUIRE(unimcu_map_size(&_ctx) == 0);
    }
}

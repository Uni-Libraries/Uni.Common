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
static unimcu_lrumap_context_t _ctx;

static unimcu_array_t _arr_link_prev{};
static size_t _arr_link_prev_buf[_capacity];

static unimcu_array_t _arr_link_next{};
static size_t _arr_link_next_buf[_capacity];

static unimcu_array_t _arr_keys{};
static size_t _arr_keys_buf[_capacity];

static unimcu_array_t _arr_vals{};
static size_t _arr_vals_buf[_capacity];


//
// Private
//

bool _lrumap_init() {
    memset(&_ctx, 0, sizeof(_ctx));

    memset(_arr_link_prev_buf, 0, sizeof(_arr_link_prev_buf));
    memset(_arr_link_next_buf, 0, sizeof(_arr_link_next_buf));
    memset(_arr_keys_buf, 0, sizeof(_arr_keys_buf));
    memset(_arr_vals_buf, 0, sizeof(_arr_vals_buf));

    memset(&_arr_link_prev, 0, sizeof(_arr_link_prev));
    memset(&_arr_link_next, 0, sizeof(_arr_link_next));
    memset(&_arr_keys, 0, sizeof(_arr_keys));
    memset(&_arr_vals, 0, sizeof(_arr_vals));

    REQUIRE_FALSE(unimcu_lrumap_initialized(&_ctx));
    unimcu_array_init(&_arr_link_prev, (uint8_t *)_arr_link_prev_buf, sizeof(_arr_link_prev_buf), sizeof(size_t));
    unimcu_array_init(&_arr_link_next, (uint8_t *)_arr_link_next_buf, sizeof(_arr_link_next_buf), sizeof(size_t));
    unimcu_array_init(&_arr_keys, (uint8_t *)_arr_keys_buf, sizeof(_arr_keys_buf), sizeof(size_t));
    unimcu_array_init(&_arr_vals, (uint8_t *)_arr_vals_buf, sizeof(_arr_vals_buf), sizeof(size_t));

    bool result = unimcu_lrumap_init(&_ctx, &_arr_link_prev, &_arr_link_next, &_arr_keys, &_arr_vals);

    REQUIRE(unimcu_lrumap_initialized(&_ctx));

    return result;
}


//
// Tests
//

TEST_CASE("lrumap_init", "[lrumap]") {
    SECTION("nullptr") { REQUIRE_FALSE(unimcu_lrumap_init(nullptr, nullptr, nullptr, nullptr, nullptr)); }

    SECTION("helper") {
        REQUIRE(_lrumap_init());
        REQUIRE(_lrumap_init());
        REQUIRE(unimcu_lrumap_length(&_ctx) == 0);
    }
}


TEST_CASE("lrumap_add", "[lrumap]") {
    SECTION("nullptr") { REQUIRE_FALSE(unimcu_lrumap_update(nullptr, 0, nullptr)); }

    SECTION("ok") {
        _lrumap_init();
        REQUIRE(unimcu_lrumap_length(&_ctx) == 0);

        size_t val = 0;
        REQUIRE(unimcu_lrumap_update(&_ctx, 0, &val));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 1);

        REQUIRE(unimcu_lrumap_update(&_ctx, 1, &val));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 2);

        REQUIRE(unimcu_lrumap_update(&_ctx, 1, &val));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 2);
    }

    SECTION("ok-2"){
        //init
        _lrumap_init();
        REQUIRE(unimcu_lrumap_length(&_ctx) == 0);
        REQUIRE(_ctx.slot_first == SIZE_MAX);
        REQUIRE(_ctx.slot_last == SIZE_MAX);

        //add 1-->2
        size_t key = 1;
        size_t val = 2;
        REQUIRE(unimcu_lrumap_update(&_ctx, key, &val));
        REQUIRE(*(size_t*)unimcu_lrumap_get(&_ctx, key) == val);
        REQUIRE(unimcu_lrumap_length(&_ctx) == 1);
        REQUIRE(_ctx.slot_first == 0);
        REQUIRE(_ctx.slot_last == 0);


        //add 2->3
        key = 2;
        val = 3;
        REQUIRE(unimcu_lrumap_update(&_ctx, key, &val));
        REQUIRE(*(size_t*)unimcu_lrumap_get(&_ctx, key) == val);
        REQUIRE(unimcu_lrumap_length(&_ctx) == 2);
        REQUIRE(_ctx.slot_first == 0);
        REQUIRE(_ctx.slot_last == 1);

        //add 1->4
        key = 1;
        val = 4;
        REQUIRE(unimcu_lrumap_update(&_ctx, key, &val));
        REQUIRE(*(size_t*)unimcu_lrumap_get(&_ctx, key) == val);
        REQUIRE(unimcu_lrumap_length(&_ctx) == 2);
        REQUIRE(_ctx.slot_first == 1);
        REQUIRE(_ctx.slot_last == 0);
    }

    SECTION("ok-3") {
        _lrumap_init();

        for (size_t idx = 0; idx < unimcu_lrumap_capacity(&_ctx); idx++) {
            REQUIRE(unimcu_lrumap_update(&_ctx, idx, &idx));
        }

        size_t val = 100;
        REQUIRE(unimcu_lrumap_update(&_ctx, val, &val));
        REQUIRE(unimcu_lrumap_get(&_ctx, 0) == nullptr);
        for (size_t idx = 1; idx < unimcu_lrumap_capacity(&_ctx); idx++) {
            REQUIRE(unimcu_lrumap_update(&_ctx, idx, &idx));
        }
    }
}


TEST_CASE("lrumap_get", "[lrumap]") {
    SECTION("nullptr") { REQUIRE(unimcu_lrumap_get(nullptr, 0) == nullptr); }

    SECTION("ok") {
        _lrumap_init();
        REQUIRE(unimcu_lrumap_length(&_ctx) == 0);
        REQUIRE(unimcu_lrumap_get(&_ctx, 0) == nullptr);
        REQUIRE(unimcu_lrumap_get_first(&_ctx) == nullptr);
        REQUIRE(unimcu_lrumap_get_last(&_ctx) == nullptr);

        size_t val = 1;
        REQUIRE(unimcu_lrumap_update(&_ctx, val - 1, &val));
        REQUIRE(*(size_t *)unimcu_lrumap_get(&_ctx, 0) == 1);
        REQUIRE(*(size_t *)unimcu_lrumap_get_first(&_ctx) == 1);
        REQUIRE(*(size_t *)unimcu_lrumap_get_last(&_ctx) == 1);

        val = 2;
        REQUIRE(unimcu_lrumap_update(&_ctx, val - 1, &val));
        REQUIRE(*(size_t *)unimcu_lrumap_get(&_ctx, val - 1) == val);
        REQUIRE(*(size_t *)unimcu_lrumap_get_first(&_ctx) == 1);
        REQUIRE(*(size_t *)unimcu_lrumap_get_last(&_ctx) == 2);

        val = 2;
        REQUIRE(unimcu_lrumap_update(&_ctx, val - 2, &val));
        REQUIRE(*(size_t *)unimcu_lrumap_get(&_ctx, val - 2) == val);
    }
}


TEST_CASE("lrumap_get_idx", "[lrumap]") {
    SECTION("nullptr") { REQUIRE_FALSE(unimcu_lrumap_get_idx(nullptr, 0, nullptr, nullptr)); }

    SECTION("ok") {
        _lrumap_init();

        size_t key = 0;
        size_t val = 0;

        // empty cache
        REQUIRE_FALSE(unimcu_lrumap_get_idx(&_ctx, unimcu_lrumap_capacity(&_ctx), &key, &val));
        REQUIRE_FALSE(unimcu_lrumap_get_idx(&_ctx, unimcu_lrumap_length(&_ctx), &key, &val));
        REQUIRE_FALSE(unimcu_lrumap_get_idx(&_ctx, 0, &key, &val));

        // set [0] - (2,4)
        key = 2;
        val = 4;
        REQUIRE(unimcu_lrumap_update(&_ctx, key, &val));

        // get [0] - (2,4)
        REQUIRE_FALSE(unimcu_lrumap_get_idx(&_ctx, unimcu_lrumap_capacity(&_ctx), &key, &val));
        REQUIRE_FALSE(unimcu_lrumap_get_idx(&_ctx, unimcu_lrumap_length(&_ctx), &key, &val));
        REQUIRE(unimcu_lrumap_get_idx(&_ctx, 0, &key, &val));
        REQUIRE(key == 2);
        REQUIRE(val == 4);

        // get [1] - not exists
        // get [2] - not exists
        REQUIRE_FALSE(unimcu_lrumap_get_idx(&_ctx, 1, &key, &val));
        REQUIRE_FALSE(unimcu_lrumap_get_idx(&_ctx, 2, &key, &val));

        // set [1] - (3,6)
        key = 3;
        val = 6;
        REQUIRE(unimcu_lrumap_update(&_ctx, key, &val));

        // get [0] - (2,4)
        REQUIRE_FALSE(unimcu_lrumap_get_idx(&_ctx, unimcu_lrumap_capacity(&_ctx), &key, &val));
        REQUIRE_FALSE(unimcu_lrumap_get_idx(&_ctx, unimcu_lrumap_length(&_ctx), &key, &val));
        REQUIRE(unimcu_lrumap_get_idx(&_ctx, 0, &key, &val));
        REQUIRE(key == 2);
        REQUIRE(val == 4);

        // get [1] - (3,6)
        REQUIRE(unimcu_lrumap_get_idx(&_ctx, 1, &key, &val));
        REQUIRE(key == 3);
        REQUIRE(val == 6);

        // get [2] -- not exists
        REQUIRE_FALSE(unimcu_lrumap_get_idx(&_ctx, 2, &key, &val));
    }
}


TEST_CASE("lrumap_remove", "[lrumap]") {
    SECTION("nullptr") { REQUIRE_FALSE(unimcu_lrumap_remove(nullptr, 0)); }

    SECTION("ok") {
        //init
        _lrumap_init();
        REQUIRE(unimcu_lrumap_length(&_ctx) == 0);

        size_t val = 0;
        REQUIRE(unimcu_lrumap_update(&_ctx, 0, &val));
        REQUIRE(unimcu_lrumap_update(&_ctx, 1, &val));
        REQUIRE(unimcu_lrumap_update(&_ctx, 2, &val));


        REQUIRE(unimcu_lrumap_length(&_ctx) == 3);

        REQUIRE_FALSE(unimcu_lrumap_remove(&_ctx, 4));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 3);

        REQUIRE(unimcu_lrumap_remove(&_ctx, 2));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 2);

        REQUIRE_FALSE(unimcu_lrumap_remove(&_ctx, 2));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 2);
    }

    SECTION("ok-first-1"){
        //init
        _lrumap_init();
        REQUIRE(unimcu_lrumap_length(&_ctx) == 0);
        REQUIRE(_ctx.slot_first == SIZE_MAX);
        REQUIRE(_ctx.slot_last == SIZE_MAX);

        //create 1->0, 2->0
        size_t val = 0;
        REQUIRE(unimcu_lrumap_update(&_ctx, 1, &val));
        REQUIRE(unimcu_lrumap_update(&_ctx, 2, &val));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 2);
        REQUIRE(_ctx.slot_first == 0);
        REQUIRE(_ctx.slot_last == 1);

        //remove 1->0
        REQUIRE(unimcu_lrumap_remove(&_ctx, 1));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 1);
        REQUIRE(_ctx.slot_first == 1);
        REQUIRE(_ctx.slot_last == 1);

        // remove 2->0
        REQUIRE(unimcu_lrumap_remove(&_ctx, 2));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 0);
        REQUIRE(_ctx.slot_first == SIZE_MAX);
        REQUIRE(_ctx.slot_last == SIZE_MAX);
    }

    SECTION("ok-first-2") {
        // init
        _lrumap_init();
        REQUIRE(unimcu_lrumap_length(&_ctx) == 0);
        REQUIRE(_ctx.slot_first == SIZE_MAX);
        REQUIRE(_ctx.slot_last == SIZE_MAX);

        // create 1->0, 2->0
        size_t val = 0;
        REQUIRE(unimcu_lrumap_update(&_ctx, 1, &val));
        REQUIRE(unimcu_lrumap_update(&_ctx, 2, &val));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 2);
        REQUIRE(_ctx.slot_first == 0);
        REQUIRE(_ctx.slot_last == 1);

        // remove 1->0
        REQUIRE(unimcu_lrumap_remove_first(&_ctx));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 1);
        REQUIRE(_ctx.slot_first == 1);
        REQUIRE(_ctx.slot_last == 1);

        // remove 2->0
        REQUIRE(unimcu_lrumap_remove_first(&_ctx));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 0);
        REQUIRE(_ctx.slot_first == SIZE_MAX);
        REQUIRE(_ctx.slot_last == SIZE_MAX);
    }

    SECTION("ok-last-1") {
        // init
        _lrumap_init();
        REQUIRE(unimcu_lrumap_length(&_ctx) == 0);
        REQUIRE(_ctx.slot_first == SIZE_MAX);
        REQUIRE(_ctx.slot_last == SIZE_MAX);

        // create 1->0, 2->0
        size_t val = 0;
        REQUIRE(unimcu_lrumap_update(&_ctx, 1, &val));
        REQUIRE(unimcu_lrumap_update(&_ctx, 2, &val));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 2);
        REQUIRE(_ctx.slot_first == 0);
        REQUIRE(_ctx.slot_last == 1);

        //remove 2->0
        REQUIRE(unimcu_lrumap_remove(&_ctx, 2));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 1);
        REQUIRE(_ctx.slot_first == 0);
        REQUIRE(_ctx.slot_last == 0);

        // remove 1->0
        REQUIRE(unimcu_lrumap_remove(&_ctx, 1));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 0);
        REQUIRE(_ctx.slot_first == SIZE_MAX);
        REQUIRE(_ctx.slot_last == SIZE_MAX);
    }

    SECTION("ok-last-2") {
        // init
        _lrumap_init();
        REQUIRE(unimcu_lrumap_length(&_ctx) == 0);
        REQUIRE(_ctx.slot_first == SIZE_MAX);
        REQUIRE(_ctx.slot_last == SIZE_MAX);

        // create 1->0, 2->0
        size_t val = 0;
        REQUIRE(unimcu_lrumap_update(&_ctx, 1, &val));
        REQUIRE(unimcu_lrumap_update(&_ctx, 2, &val));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 2);
        REQUIRE(_ctx.slot_first == 0);
        REQUIRE(_ctx.slot_last == 1);

        // remove 2->0
        REQUIRE(unimcu_lrumap_remove_last(&_ctx));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 1);
        REQUIRE(_ctx.slot_first == 0);
        REQUIRE(_ctx.slot_last == 0);

        // remove 1->0
        REQUIRE(unimcu_lrumap_remove_last(&_ctx));
        REQUIRE(unimcu_lrumap_length(&_ctx) == 0);
        REQUIRE(_ctx.slot_first == SIZE_MAX);
        REQUIRE(_ctx.slot_last == SIZE_MAX);
    }
}


TEST_CASE("lrumap_empty", "[lrumap]") {
    SECTION("nullptr") { REQUIRE_FALSE(unimcu_lrumap_empty(nullptr)); }

    SECTION("ok") {
        _lrumap_init();
        REQUIRE(unimcu_lrumap_empty(&_ctx));

        size_t val = 0;
        REQUIRE(unimcu_lrumap_update(&_ctx, 0, &val));
        REQUIRE_FALSE(unimcu_lrumap_empty(&_ctx));

        REQUIRE(unimcu_lrumap_remove(&_ctx, 0));
        REQUIRE(unimcu_lrumap_empty(&_ctx));
    }
}


TEST_CASE("lrumap_enum", "[lrumap]") {
    SECTION("nullptr_obj") { REQUIRE_FALSE(unimcu_lrumap_enum(nullptr, nullptr)); }

    SECTION("nullptr_func") {
        _lrumap_init();
        REQUIRE_FALSE(unimcu_lrumap_enum(&_ctx, nullptr));
    }

    SECTION("ok") {
        _lrumap_init();

        size_t capacity = unimcu_lrumap_capacity(&_ctx);
        for (size_t idx = 1; idx <= capacity; idx++) {
            REQUIRE(unimcu_lrumap_update(&_ctx, idx - 1, &idx));
        }

        _temp = 0;
        REQUIRE(unimcu_lrumap_enum(&_ctx, [](size_t key, const void *val) {
            REQUIRE((key == _temp));
            REQUIRE(((*(const uint32_t *)val) == _temp + 1));
            _temp++;
        }));

        size_t val = capacity + 1;
        REQUIRE(unimcu_lrumap_update(&_ctx, val - 1, &val));

        _temp = 1;
        REQUIRE(unimcu_lrumap_enum(&_ctx, [](size_t key, const void *val) {
            REQUIRE((key == _temp));
            REQUIRE(((*(const uint32_t *)val) == _temp + 1));
            _temp++;
        }));
    }
}


TEST_CASE("lrumap_clear", "[lrumap]") {
    SECTION("nullptr") { REQUIRE_FALSE(unimcu_lrumap_clear(nullptr)); }

    SECTION("ok") {
        _lrumap_init();

        size_t val = 0;
        REQUIRE(unimcu_lrumap_update(&_ctx, 0, &val));
        REQUIRE_FALSE(unimcu_lrumap_empty(&_ctx));

        REQUIRE(unimcu_lrumap_clear(&_ctx));
        REQUIRE(unimcu_lrumap_empty(&_ctx));
    }
}


TEST_CASE("lrumap_displacement", "[lrumap]") {
    _lrumap_init();

    size_t key = 1;
    size_t val = 11;
    REQUIRE(unimcu_lrumap_update(&_ctx, key, &val));

    key = 2;
    val = 22;
    REQUIRE(unimcu_lrumap_update(&_ctx, key, &val));

    key = 3;
    val = 33;
    REQUIRE(unimcu_lrumap_update(&_ctx, key, &val));

    key = 0;
    val = 0;
    REQUIRE(unimcu_lrumap_get_idx(&_ctx, 0, &key, &val));
    REQUIRE((key == 1 && val == 11));
    REQUIRE(unimcu_lrumap_get_idx(&_ctx, 1, &key, &val));
    REQUIRE((key == 2 && val == 22));
    REQUIRE(unimcu_lrumap_get_idx(&_ctx, 2, &key, &val));
    REQUIRE((key == 3 && val == 33));

    key = 1;
    val = 333;
    REQUIRE(unimcu_lrumap_update(&_ctx, key, &val));
    REQUIRE(unimcu_lrumap_get_idx(&_ctx, 0, &key, &val));
    REQUIRE((key == 2 && val == 22));
    REQUIRE(unimcu_lrumap_get_idx(&_ctx, 1, &key, &val));
    REQUIRE((key == 3 && val == 33));
    REQUIRE(unimcu_lrumap_get_idx(&_ctx, 2, &key, &val));
    REQUIRE((key == 1 && val == 333));
}

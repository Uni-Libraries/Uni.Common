//
// Includes
//

#include <stdbool.h>
#include <string.h>

#include "unimcu_common_compiler.h"
#include "unimcu_common_map.h"
#include "unimcu_common_math.h"


//
// Static globals
//

static const size_t _sizemax = SIZE_MAX;


//
// Private functions
//


/**
 * Clears give map
 * @param ctx pointer to the map context
 */
static void _unimcu_map_clear(unimcu_map_context_t *ctx) {
    unimcu_array_fill(ctx->config.keys, 0xFF);
    unimcu_array_fill(ctx->config.vals, 0xFF);
    ctx->state.capacity = 0U;
    ctx->state.size = 0U;
}


/**
 * Gets array index for the given object ID
 * @param ctx pointer to the LRU cache context
 * @param key key of the object
 * @return index of the object, SIZE_MAX if element was not found
 *
 * @note input data must be valid
 */
static size_t _unimcu_map_get_slot_bykey(unimcu_map_context_t *ctx, size_t key) {
    size_t result = SIZE_MAX;

    size_t capacity = unimcu_map_capacity(ctx);
    for (size_t slot = 0; slot < capacity; slot++) {
        size_t *slot_key = (size_t *)unimcu_array_get(ctx->config.keys, slot);
        if (*slot_key == key) {
            result = slot;
            break;
        }
    }

    return result;
}


/**
 * Get first empty slot
 * @param ctx pointer to the LRU context
 * @return index of the first empty slot, SIZE_MAX if there is no empty slots
 *
 * @note input data must be valid
 */
static size_t _unimcu_map_get_slot_empty(unimcu_map_context_t *ctx) {
    size_t result = SIZE_MAX;

    size_t capacity = unimcu_map_capacity(ctx);
    for (size_t slot = 0; slot < capacity; slot++) {
        if (*(size_t*)unimcu_array_get(ctx->config.keys, slot) == SIZE_MAX) {
            result = slot;
            break;
        }
    }

    return result;
}


/**
 * Removes the given slot from the LRU
 * @param ctx pointer to the LRU context
 * @param slot slot number
 *
 * @note it just relinks neighbors and first/last slot number, the content in key and value arrays will be unchanged
 * @note input data must be valid
 */
static void _unimcu_map_remove_slot(unimcu_map_context_t *ctx, size_t slot) {
    *(size_t*)unimcu_array_get(ctx->config.keys, slot) = SIZE_MAX;
}


/**
 * Sets context of the given slot
 * @param ctx pointer to the LRU context
 * @param slot slot number
 * @param key key value
 * @param val data value
 *
 * @note input data must be valid
 *
 * @return true on success
 */
static void _unimcu_map_set_slot(unimcu_map_context_t *ctx, size_t slot, size_t key, const void *val) {
    unimcu_array_set(ctx->config.keys, slot, (uint8_t *)&key);
    unimcu_array_set(ctx->config.vals, slot, val);
}



//
// Functions/Init
//

bool unimcu_map_init(unimcu_map_context_t *ctx, unimcu_array_t *keys, unimcu_array_t *vals) {
    bool result = false;

    if (ctx != NULL && keys != NULL && vals != NULL) {
        ctx->config.keys = keys;
        ctx->config.vals = vals;
        unimcu_array_set_itemsize(ctx->config.keys, sizeof(size_t));
        _unimcu_map_clear(ctx);
        ctx->state.capacity = unimcu_math_min(unimcu_array_length(ctx->config.keys), unimcu_array_length((ctx->config.vals)));
        ctx->state.initialized = true;
        result = true;
    }

    return result;
}


//
// Functions/Getters
//


size_t unimcu_map_capacity(const unimcu_map_context_t *ctx) {
    size_t result = 0U;

    if (unimcu_map_initialized(ctx)) {
        result = ctx->state.capacity;
    }

    return result;
}


bool unimcu_map_initialized(const unimcu_map_context_t *ctx) {
    bool result = false;
    if (ctx != NULL) {
        result = ctx->state.initialized;
    }
    return result;
}


size_t unimcu_map_size(const unimcu_map_context_t *ctx) {
    size_t result = 0U;

    if (unimcu_map_initialized(ctx)) {
        result = ctx->state.size;
    }

    return result;
}


//
// Functions/Process
//

bool unimcu_map_clear(unimcu_map_context_t *ctx) {
    bool result = false;

    if (unimcu_map_initialized(ctx)) {
        _unimcu_map_clear(ctx);
        result = true;
    }

    return result;
}


bool unimcu_map_enum(unimcu_map_context_t *ctx, unimcu_map_enum_func_t func) {
    bool result = false;

    if (unimcu_map_initialized(ctx) && func != NULL) {
        for(size_t idx = 0U; idx < ctx->state.capacity; idx++) {
            size_t slot_key = *(size_t *)unimcu_array_get(ctx->config.keys, idx);
            if (slot_key != SIZE_MAX) {
                func(slot_key, unimcu_array_get(ctx->config.vals, idx));
            }
        }
        result = true;
    }

    return result;
}


uint8_t *unimcu_map_get(unimcu_map_context_t *ctx, size_t key) {
    uint8_t *result = NULL;

    if (unimcu_map_initialized(ctx)) {
        size_t slot = _unimcu_map_get_slot_bykey(ctx, key);
        if (slot != SIZE_MAX) {
            result = unimcu_array_get(ctx->config.vals, slot);
        }
    }

    return result;
}


bool unimcu_map_remove(unimcu_map_context_t *ctx, size_t key) {
    bool result = false;

    if (unimcu_map_initialized(ctx)) {
        size_t slot = _unimcu_map_get_slot_bykey(ctx, key);
        if (slot != SIZE_MAX) {
            _unimcu_map_remove_slot(ctx, slot);
            ctx->state.size--;
            result = true;
        }
    }

    return result;
}


bool unimcu_map_set(unimcu_map_context_t *ctx, size_t key, const void *val) {
    bool result = false;

    size_t idx = SIZE_MAX;
    bool newrecord = false;

    if (unimcu_map_initialized(ctx)) {
        // find if it exists
        idx = _unimcu_map_get_slot_bykey(ctx, key);
        if (idx == SIZE_MAX && ctx->state.size < ctx->state.capacity) {
            idx = _unimcu_map_get_slot_empty(ctx);
            newrecord = true;
        }

        if(idx != SIZE_MAX){
            _unimcu_map_set_slot(ctx, idx, key, val);
            result = true;
            if(newrecord) {
                ctx->state.size++;
            }
        }
    }

    return result;
}

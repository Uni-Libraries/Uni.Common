//
// Includes
//

#include <stdbool.h>
#include <string.h>

#include "uni_common_compiler.h"
#include "uni_common_map.h"
#include "uni_common_math.h"



//
// Private functions
//


/**
 * Clears give map
 * @param ctx pointer to the map context
 */
static void _uni_common_map_clear(uni_common_map_context_t *ctx) {
    uni_common_array_fill(ctx->config.keys, 0xFF);
    uni_common_array_fill(ctx->config.vals, 0xFF);
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
static size_t _uni_common_map_get_slot_bykey(uni_common_map_context_t *ctx, size_t key) {
    size_t result = SIZE_MAX;

    size_t capacity = uni_common_map_capacity(ctx);
    for (size_t slot = 0; slot < capacity; slot++) {
        size_t *slot_key = (size_t *)uni_common_array_get(ctx->config.keys, slot);
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
static size_t _uni_common_map_get_slot_empty(uni_common_map_context_t *ctx) {
    size_t result = SIZE_MAX;

    size_t capacity = uni_common_map_capacity(ctx);
    for (size_t slot = 0; slot < capacity; slot++) {
        if (*(size_t*)uni_common_array_get(ctx->config.keys, slot) == SIZE_MAX) {
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
static void _uni_common_map_remove_slot(uni_common_map_context_t *ctx, size_t slot) {
    *(size_t*)uni_common_array_get(ctx->config.keys, slot) = SIZE_MAX;
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
static void _uni_common_map_set_slot(uni_common_map_context_t *ctx, size_t slot, size_t key, const void *val) {
    uni_common_array_set(ctx->config.keys, slot, (uint8_t *)&key);
    uni_common_array_set(ctx->config.vals, slot, val);
}



//
// Functions/Init
//

bool uni_common_map_init(uni_common_map_context_t *ctx, uni_common_array_t *keys, uni_common_array_t *vals) {
    bool result = false;

    if (ctx != NULL && keys != NULL && vals != NULL) {
        ctx->config.keys = keys;
        ctx->config.vals = vals;
        uni_common_array_set_itemsize(ctx->config.keys, sizeof(size_t));
        _uni_common_map_clear(ctx);
        ctx->state.capacity = uni_common_math_min(uni_common_array_length(ctx->config.keys), uni_common_array_length((ctx->config.vals)));
        ctx->state.initialized = true;
        result = true;
    }

    return result;
}


//
// Functions/Getters
//


size_t uni_common_map_capacity(const uni_common_map_context_t *ctx) {
    size_t result = 0U;

    if (uni_common_map_initialized(ctx)) {
        result = ctx->state.capacity;
    }

    return result;
}


bool uni_common_map_initialized(const uni_common_map_context_t *ctx) {
    bool result = false;
    if (ctx != NULL) {
        result = ctx->state.initialized;
    }
    return result;
}


size_t uni_common_map_size(const uni_common_map_context_t *ctx) {
    size_t result = 0U;

    if (uni_common_map_initialized(ctx)) {
        result = ctx->state.size;
    }

    return result;
}


//
// Functions/Process
//

bool uni_common_map_clear(uni_common_map_context_t *ctx) {
    bool result = false;

    if (uni_common_map_initialized(ctx)) {
        _uni_common_map_clear(ctx);
        result = true;
    }

    return result;
}


bool uni_common_map_enum(uni_common_map_context_t *ctx, uni_common_map_enum_func_t func) {
    bool result = false;

    if (uni_common_map_initialized(ctx) && func != NULL) {
        for(size_t idx = 0U; idx < ctx->state.capacity; idx++) {
            size_t slot_key = *(size_t *)uni_common_array_get(ctx->config.keys, idx);
            if (slot_key != SIZE_MAX) {
                func(slot_key, uni_common_array_get(ctx->config.vals, idx));
            }
        }
        result = true;
    }

    return result;
}


uint8_t *uni_common_map_get(uni_common_map_context_t *ctx, size_t key) {
    uint8_t *result = NULL;

    if (uni_common_map_initialized(ctx)) {
        size_t slot = _uni_common_map_get_slot_bykey(ctx, key);
        if (slot != SIZE_MAX) {
            result = uni_common_array_get(ctx->config.vals, slot);
        }
    }

    return result;
}


bool uni_common_map_remove(uni_common_map_context_t *ctx, size_t key) {
    bool result = false;

    if (uni_common_map_initialized(ctx)) {
        size_t slot = _uni_common_map_get_slot_bykey(ctx, key);
        if (slot != SIZE_MAX) {
            _uni_common_map_remove_slot(ctx, slot);
            ctx->state.size--;
            result = true;
        }
    }

    return result;
}


bool uni_common_map_set(uni_common_map_context_t *ctx, size_t key, const void *val) {
    bool result = false;

    size_t idx = SIZE_MAX;
    bool newrecord = false;

    if (uni_common_map_initialized(ctx)) {
        // find if it exists
        idx = _uni_common_map_get_slot_bykey(ctx, key);
        if (idx == SIZE_MAX && ctx->state.size < ctx->state.capacity) {
            idx = _uni_common_map_get_slot_empty(ctx);
            newrecord = true;
        }

        if(idx != SIZE_MAX){
            _uni_common_map_set_slot(ctx, idx, key, val);
            result = true;
            if(newrecord) {
                ctx->state.size++;
            }
        }
    }

    return result;
}

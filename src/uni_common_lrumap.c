//
// Includes
//

#include <stdbool.h>
#include <string.h>

#include "uni_common_compiler.h"
#include "uni_common_lrumap.h"
#include "uni_common_math.h"


//
// Static globals
//

static const size_t _sizemax = SIZE_MAX;


//
// Private functions
//


/**
 * Clears give LRUmap
 * @param ctx pointer to the LRUmap context
 */
static void _uni_common_lrumap_clear(uni_common_lrumap_context_t *ctx) {
    uni_common_array_fill(ctx->arr_link_prev, 0xFF);
    uni_common_array_fill(ctx->arr_link_next, 0xFF);
    uni_common_array_fill(ctx->arr_keys, 0xFF);

    ctx->slot_last = SIZE_MAX;
    ctx->slot_first = SIZE_MAX;
}


/**
 * Checks that LRU map is empty
 * @param ctx pointer to the LRU cache context
 * @return true if map is empty
 *
 * @note input data must be valid
 */
static bool _uni_common_lrumap_empty(const uni_common_lrumap_context_t *ctx){
    return ctx->slot_first == SIZE_MAX && ctx->slot_last == SIZE_MAX;
}


/**
 * Gets array index for the given object ID
 * @param ctx pointer to the LRU cache context
 * @param key key of the object
 * @return index of the object, SIZE_MAX if element was not found
 *
 * @note input data must be valid
 */
static size_t _uni_common_lrumap_get_slot_bykey(uni_common_lrumap_context_t *ctx, size_t key) {
    size_t result = SIZE_MAX;

    size_t capacity = uni_common_lrumap_capacity(ctx);
    for (size_t slot = 0; slot < capacity; slot++) {
        size_t *slot_key = (size_t *)uni_common_array_get(ctx->arr_keys, slot);
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
static size_t _uni_common_lrumap_get_slot_empty(uni_common_lrumap_context_t *ctx) {
    size_t result = SIZE_MAX;

    if (_uni_common_lrumap_empty(ctx)) {
        result = 0U;
    } else {
        size_t capacity = uni_common_lrumap_capacity(ctx);
        for (size_t slot = 0; slot < capacity; slot++) {
            size_t slot_prev = *(size_t *)uni_common_array_get(ctx->arr_link_prev, slot);
            size_t slot_next = *(size_t *)uni_common_array_get(ctx->arr_link_next, slot);

            if (slot != ctx->slot_first && slot != ctx->slot_last && slot_prev == SIZE_MAX &&
                slot_next == SIZE_MAX) {
                result = slot;
                break;
            }
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
static void _uni_common_lrumap_remove_slot(uni_common_lrumap_context_t *ctx, size_t slot) {
    // get indexes of prev and next els
    size_t slot_prev = *(size_t *)uni_common_array_get(ctx->arr_link_prev, slot);
    size_t slot_next = *(size_t *)uni_common_array_get(ctx->arr_link_next, slot);

    // connect previous one with the next one
    if (slot_prev != SIZE_MAX) {
        uni_common_array_set(ctx->arr_link_next, slot_prev, (uint8_t *)&slot_next);
    }
    if (slot_next != SIZE_MAX) {
        uni_common_array_set(ctx->arr_link_prev, slot_next, (uint8_t *)&slot_prev);
    }

    // update first and last elements if needed
    if (ctx->slot_first == slot) {
        ctx->slot_first = slot_next;
    }
    if (ctx->slot_last == slot) {
        ctx->slot_last = slot_prev;
    }

    // mark current index as orphan
    uni_common_array_set(ctx->arr_link_prev, slot, (const uint8_t *)&_sizemax);
    uni_common_array_set(ctx->arr_link_next, slot, (const uint8_t *)&_sizemax);
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
static void _uni_common_lrumap_set_slot(uni_common_lrumap_context_t *ctx, size_t slot, size_t key, const void *val) {
    uni_common_array_set(ctx->arr_keys, slot, (uint8_t *)&key);
    uni_common_array_set(ctx->arr_vals, slot, val);
}


/**
 * Appends given slot to the end of the list
 *
 * @param ctx pointer to the LRU cache context
 * @param slot number of slot which should be appended
 *
 * @note input data must be valid
 */
static void _uni_common_lrumap_append_slot(uni_common_lrumap_context_t *ctx, size_t slot) {
    if (ctx->slot_first == SIZE_MAX) {
        ctx->slot_first = slot;
    }

    if (ctx->slot_last == SIZE_MAX) {
        ctx->slot_last = slot;
    } else {
        uni_common_array_set(ctx->arr_link_next, ctx->slot_last, (uint8_t *)&slot);
        uni_common_array_set(ctx->arr_link_next, slot, (const uint8_t *)&_sizemax);
        uni_common_array_set(ctx->arr_link_prev, slot, (uint8_t *)&ctx->slot_last);
        ctx->slot_last = slot;
    }
}


/**
 * Moves slot to the last position in list
 * @param ctx pointer to the LRU cache context
 * @param slot slot number
 *
 * @note input data must be valid
 */
static void _uni_common_lrumap_refresh_slot(uni_common_lrumap_context_t *ctx, size_t slot) {
    _uni_common_lrumap_remove_slot(ctx, slot);
    _uni_common_lrumap_append_slot(ctx, slot);
}


//
// Functions/Init
//

bool uni_common_lrumap_init(uni_common_lrumap_context_t *ctx, uni_common_array_t *arr_link_prev, uni_common_array_t *arr_link_next,
                          uni_common_array_t *arr_keys, uni_common_array_t *arr_vals) {
    bool result = false;

    if (ctx != NULL && arr_link_prev != NULL && arr_link_next != NULL && arr_keys != NULL && arr_vals != NULL) {
        ctx->arr_link_prev = arr_link_prev;
        ctx->arr_link_next = arr_link_next;
        ctx->arr_keys = arr_keys;
        ctx->arr_vals = arr_vals;

        uni_common_array_set_itemsize(ctx->arr_link_next, sizeof(size_t));
        uni_common_array_set_itemsize(ctx->arr_link_prev, sizeof(size_t));
        uni_common_array_set_itemsize(ctx->arr_keys, sizeof(size_t));

        _uni_common_lrumap_clear(ctx);

        ctx->initialized = true;
        result = true;
    }

    return result;
}


//
// Functions/Getters
//


size_t uni_common_lrumap_capacity(const uni_common_lrumap_context_t *ctx) {
    size_t result = 0U;

    if (uni_common_lrumap_initialized(ctx)) {
        result = uni_common_array_length(ctx->arr_link_prev);
        result = uni_common_math_min(result, uni_common_array_length(ctx->arr_link_next));
        result = uni_common_math_min(result, uni_common_array_length(ctx->arr_keys));
        result = uni_common_math_min(result, uni_common_array_length(ctx->arr_vals));
    }

    return result;
}


bool uni_common_lrumap_empty(const uni_common_lrumap_context_t *ctx) {
    bool result = false;

    if (uni_common_lrumap_initialized(ctx)) {
        result = _uni_common_lrumap_empty(ctx);
    }

    return result;
}


bool uni_common_lrumap_initialized(const uni_common_lrumap_context_t *ctx) {
    bool result = false;

    if (ctx != NULL) {
        result = ctx->initialized;
    }

    return result;
}


size_t uni_common_lrumap_length(const uni_common_lrumap_context_t *ctx) {
    size_t result = 0U;

    if (uni_common_lrumap_initialized(ctx)) {
        size_t slot = ctx->slot_first;
        while (slot != SIZE_MAX) {
            slot = *(size_t *)uni_common_array_get(ctx->arr_link_next, slot);
            result++;
        }
    }

    return result;
}


//
// Functions/Process
//

bool uni_common_lrumap_clear(uni_common_lrumap_context_t *ctx) {
    bool result = false;

    if (uni_common_lrumap_initialized(ctx)) {
        _uni_common_lrumap_clear(ctx);
        result = true;
    }

    return result;
}


bool uni_common_lrumap_enum(uni_common_lrumap_context_t *ctx, uni_common_lrumap_enum_func_t func) {
    bool result = false;

    if (uni_common_lrumap_initialized(ctx) && func != NULL) {
        size_t slot = ctx->slot_first;
        while (slot != SIZE_MAX) {
            size_t *slot_key = (size_t *)uni_common_array_get(ctx->arr_keys, slot);
            func(*slot_key, uni_common_array_get(ctx->arr_vals, slot));

            slot = *(size_t *)uni_common_array_get(ctx->arr_link_next, slot);
        }
        result = true;
    }

    return result;
}


uint8_t *uni_common_lrumap_get(uni_common_lrumap_context_t *ctx, size_t key) {
    uint8_t *result = NULL;

    if (uni_common_lrumap_initialized(ctx)) {
        size_t slot = _uni_common_lrumap_get_slot_bykey(ctx, key);
        if (slot != SIZE_MAX) {
            result = uni_common_array_get(ctx->arr_vals, slot);
        }
    }

    return result;
}


uint8_t *uni_common_lrumap_get_first(uni_common_lrumap_context_t *ctx) {
    uint8_t *result = NULL;

    if (uni_common_lrumap_initialized(ctx)) {
        if (ctx->slot_first != SIZE_MAX) {
            result = uni_common_array_get(ctx->arr_vals, ctx->slot_first);
        }
    }

    return result;
}


uint8_t *uni_common_lrumap_get_last(uni_common_lrumap_context_t *ctx) {
    uint8_t *result = NULL;

    if (uni_common_lrumap_initialized(ctx)) {
        if (ctx->slot_last != SIZE_MAX) {
            result = uni_common_array_get(ctx->arr_vals, ctx->slot_last);
        }
    }

    return result;
}

bool uni_common_lrumap_get_idx(uni_common_lrumap_context_t *ctx, size_t idx, size_t *key, void *val) {
    bool result = false;

    if (uni_common_lrumap_initialized(ctx) && idx < uni_common_lrumap_capacity(ctx) && (key != NULL || val != NULL) && ctx->slot_first != SIZE_MAX) {
        size_t slot = ctx->slot_first;

        size_t i = 0;
        while (i < idx) {
            slot = *(size_t *)uni_common_array_get(ctx->arr_link_next, slot);
            if (slot == SIZE_MAX) {
                break;
            }
            i++;
        }

        if (i == idx) {
            if (key != NULL) {
                memcpy(key, uni_common_array_get(ctx->arr_keys, slot), uni_common_array_itemsize(ctx->arr_keys));
            }

            if (val != NULL) {
                memcpy(val, uni_common_array_get(ctx->arr_vals, slot), uni_common_array_itemsize(ctx->arr_vals));
            }

            result = true;
        }
    }

    return result;
}


bool uni_common_lrumap_remove(uni_common_lrumap_context_t *ctx, size_t key) {
    bool result = false;

    if (uni_common_lrumap_initialized(ctx)) {
        size_t slot = _uni_common_lrumap_get_slot_bykey(ctx, key);
        if (slot != SIZE_MAX) {
            // unlink slot
            _uni_common_lrumap_remove_slot(ctx, slot);

            // mark key as non-existent
            uni_common_array_set(ctx->arr_keys, slot, (const uint8_t *)&_sizemax);

            result = true;
        }
    }

    return result;
}


bool uni_common_lrumap_remove_first(uni_common_lrumap_context_t *ctx) {
    bool result = false;

    if (uni_common_lrumap_initialized(ctx)) {
        size_t slot_target = ctx->slot_first;

        if (slot_target != SIZE_MAX) {
            // unlink slot
            _uni_common_lrumap_remove_slot(ctx, slot_target);

            // mark key as non-existent
            uni_common_array_set(ctx->arr_keys, slot_target, (const uint8_t *)&_sizemax);

            result = true;
        }
    }

    return result;
}


bool uni_common_lrumap_remove_last(uni_common_lrumap_context_t *ctx) {
    bool result = false;

    if (uni_common_lrumap_initialized(ctx)) {
        size_t slot_target = ctx->slot_last;

        if (slot_target != SIZE_MAX) {
            // unlink slot
            _uni_common_lrumap_remove_slot(ctx, slot_target);

            // mark key as non-existent
            uni_common_array_set(ctx->arr_keys, slot_target, (const uint8_t *)&_sizemax);

            result = true;
        }
    }

    return result;
}


bool uni_common_lrumap_update(uni_common_lrumap_context_t *ctx, size_t key, const void *val) {
    bool result = false;

    size_t idx = SIZE_MAX;

    if (uni_common_lrumap_initialized(ctx)) {
        // find if it exists
        idx = _uni_common_lrumap_get_slot_bykey(ctx, key);
        if (idx != SIZE_MAX) {
            _uni_common_lrumap_set_slot(ctx, idx, key, val);
            _uni_common_lrumap_refresh_slot(ctx, idx);
            result = true;
        }

        // find empty element
        if (!result) {
            idx = _uni_common_lrumap_get_slot_empty(ctx);
            if (idx != SIZE_MAX) {
                _uni_common_lrumap_set_slot(ctx, idx, key, val);
                _uni_common_lrumap_append_slot(ctx, idx);
                result = true;
            }
        }

        // replace first element
        if (!result) {
            idx = ctx->slot_first;
            if (idx != SIZE_MAX) {
                _uni_common_lrumap_set_slot(ctx, idx, key, val);
                _uni_common_lrumap_refresh_slot(ctx, idx);
                result = true;
            }
        }
    }

    return result;
}

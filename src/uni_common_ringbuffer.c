//
// Includes
//

#include <stdbool.h>
#include <string.h>

#include "uni_common_compiler.h"
#include "uni_common_math.h"
#include "uni_common_ringbuffer.h"


//
// Functions/Private
//

/**
 * Increment position by one object
 * @param ctx pointer to the ringbuffer object
 * @param pos position to increment
 * @return incremented position
 *
 * @note ringbuffer must be valid
 */
static size_t _uni_common_ringbuffer_pos_increment(const uni_common_ringbuffer_context_t *ctx, size_t pos) {
    return (pos + ctx->size_object) % ctx->size_total;
}

/**
 * Calculates number of written bytes into the ringbuffer
 * @param ctx pointer to the ringbuffer context
 * @param pos_front position which is used for calculation as front pos
 * @return number of written bytes
 *
 * @note ringbuffer must be valid
 */
static size_t _uni_common_ringbuffer_count_bytes(const uni_common_ringbuffer_context_t *ctx, size_t pos_front) {
    size_t result = 0U;

    if (ctx->pos_back >= pos_front) {
        result = ctx->pos_back - pos_front;
    } else {
        result = ctx->pos_back + (ctx->size_total - pos_front);
    }

    return result;
}


/**
 * Calculates number of written objects into the ringbuffer
 * @param ctx pointer to the ringbuffer context
 * @param pos_front position which is used for calculation as front pos
 * @return number of written objects
 *
 * @note ringbuffer must be valid
 */
static size_t _uni_common_ringbuffer_count_objects(const uni_common_ringbuffer_context_t *ctx, size_t pos_front) {
    return _uni_common_ringbuffer_count_bytes(ctx, pos_front) / ctx->size_object;
}


/**
 * Checks that ringbuffer is empty
 * @param ctx pointer to the ringbuffer
 * @param pos_front position which is used for calculation as front pos
 * @return true if empty
 */
static bool _uni_common_ringbuffer_is_empty(const uni_common_ringbuffer_context_t *ctx, size_t pos_front) {
    return pos_front == ctx->pos_back;
}


//
// Functions/Init
//

bool uni_common_ringbuffer_init(uni_common_ringbuffer_context_t *ctx, uint8_t *data, uint32_t size_object,
                            uint32_t size_total) {
    bool result = false;

    if (ctx != NULL && data != NULL && (size_total % size_object == 0U)) {
        ctx->data = data;

        ctx->size_object = size_object;
        ctx->size_total = size_total;

        uni_common_ringbuffer_clear(ctx);
        result = true;
    }

    return result;
}


//
// Functions/Getters
//

bool uni_common_ringbuffer_get(const uni_common_ringbuffer_context_t *ctx, size_t index, uint8_t *data) {
    bool result = false;
    if (ctx != NULL && data != NULL && index < uni_common_ringbuffer_length(ctx)) {
        // calculate position
        size_t pos = ctx->pos_front;
        for (size_t i = 0; i < index; i++) {
            pos = _uni_common_ringbuffer_pos_increment(ctx, pos);
        }

        // copy data
        (void) memcpy(data, &ctx->data[pos], ctx->size_object);

        result = true;
    }
    return result;
}


size_t uni_common_ringbuffer_find(const uni_common_ringbuffer_context_t *ctx, const uint8_t *data) {
    size_t result = SIZE_MAX;
    size_t counter = 0U;

    if (ctx != NULL && data != NULL) {
        size_t pos_current = ctx->pos_front;

        while (!_uni_common_ringbuffer_is_empty(ctx, pos_current)) {
            // compare data
            if (memcmp(data, &ctx->data[pos_current], ctx->size_object) == 0) {
                result = counter;
                break;
            }

            // increment
            pos_current = _uni_common_ringbuffer_pos_increment(ctx, pos_current);
            counter++;
        }
    }

    return result;
}


bool uni_common_ringbuffer_is_empty(const uni_common_ringbuffer_context_t *ctx) {
    bool result = false;

    if (ctx != NULL) {
        result = _uni_common_ringbuffer_is_empty(ctx, ctx->pos_front);
    }

    return result;
}


bool uni_common_ringbuffer_is_full(const uni_common_ringbuffer_context_t *ctx) {
    bool result = false;

    if (ctx != NULL) {
        result = _uni_common_ringbuffer_pos_increment(ctx, ctx->pos_back) == ctx->pos_front;
    }

    return result;
}


size_t uni_common_ringbuffer_length(const uni_common_ringbuffer_context_t *ctx) {
    uint32_t result = 0;

    if (ctx != NULL) {
        result = _uni_common_ringbuffer_count_objects(ctx, ctx->pos_front);
    }

    return result;
}


//
// Functions/Actions
//

bool uni_common_ringbuffer_clear(uni_common_ringbuffer_context_t *ctx) {
    bool result = false;

    if (ctx != NULL) {
        ctx->pos_front = 0U;
        ctx->pos_back = 0U;
        result = true;
    }

    return result;
}


size_t uni_common_ringbuffer_pop(uni_common_ringbuffer_context_t *ctx, uint8_t *data, size_t count) {
    size_t result = 0;

    if (ctx != NULL) {
        while (result < count && !_uni_common_ringbuffer_is_empty(ctx, ctx->pos_front)) {
            // copy object
            if (data != NULL) {
                (void) memcpy(data, &ctx->data[ctx->pos_front], ctx->size_object);
            }

            // update pos_front
            ctx->pos_front = _uni_common_ringbuffer_pos_increment(ctx, ctx->pos_front);

            // update counter
            result++;

            // update pointer for the next copy
            if (data != NULL) {
                data = &data[ctx->size_object];
            }
        }
    }

    return result;
}


size_t uni_common_ringbuffer_push(uni_common_ringbuffer_context_t *ctx, const uint8_t *data, size_t count) {
    size_t result = 0U;

    if (ctx != NULL && ctx->data != NULL && data != NULL) {
        while (result < count) {
            // copy object
            (void) memcpy(&ctx->data[ctx->pos_back], data, ctx->size_object);

            // calculate next back position
            size_t pos_back_next = _uni_common_ringbuffer_pos_increment(ctx, ctx->pos_back);

            // update pos_front to the previous back position if it necessary
            if (pos_back_next == ctx->pos_front) {
                ctx->pos_front = _uni_common_ringbuffer_pos_increment(ctx, ctx->pos_front);
            }

            // update back position, buffer pointer and counter
            ctx->pos_back = pos_back_next;
            data = &data[ctx->size_object];
            result++;
        }
    }

    return result;
}

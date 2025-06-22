//
// Includes
//

// stdlib
#include <stdlib.h>
#include <string.h>

// uni.common
#include "uni_common_array.h"



//
// Functions
//

uni_common_array_t * uni_common_array_create(size_t item_count, size_t item_size) {
    uni_common_array_t* result = NULL;
    if(item_size > 0 && item_count > 0) {
        result = malloc(sizeof(uni_common_array_t));
        if(result) {
            result->data = calloc(item_count,item_size);
            result->size = item_count * item_size;
            result->capacity = result->size;
            result->size_item = item_size;
            if(!result->data) {
                free(result);
                result = NULL;
            }
        }
    }
    return result;
}

bool uni_common_array_free(uni_common_array_t *ctx) {
    bool result = false;
    if(ctx != NULL) {
        if(ctx->data != NULL) {
            free(ctx->data);
        }
        free(ctx);
        result = true;
    }
    return result;
}

bool uni_common_array_init(uni_common_array_t *ctx, void *buf, size_t buf_size, size_t item_size) {
    bool result = false;

    if (ctx != NULL && buf != NULL && buf_size > 0U && item_size > 0U) {
        ctx->data = buf;
        ctx->size = buf_size;
        ctx->capacity = buf_size;
        ctx->size_item = item_size;
        result = true;
    }

    return result;
}

bool uni_common_array_fill(uni_common_array_t *ctx, uint8_t pattern) {
    bool result = false;

    if (uni_common_array_valid(ctx)) {
        memset(ctx->data, pattern, ctx->capacity);
        result = true;
    }

    return result;
}

bool uni_common_array_valid(const uni_common_array_t *arr) {
    bool result = false;

    if (arr != NULL && arr->data != NULL && arr->capacity > 0U && arr->size_item > 0U) {
        result = true;
    }

    return result;
}


bool uni_common_array_clear(uni_common_array_t *ctx) {
    bool result = false;

    if(ctx) {
        ctx->size = 0;
        result = true;
    }

    return result;
}

void *uni_common_array_data(uni_common_array_t *ctx) {
    void *result = NULL;

    if (ctx != NULL) {
        result = ctx->data;
    }

    return result;
}


size_t uni_common_array_capacity(const uni_common_array_t *ctx) {
    size_t result = 0;
    if (ctx != NULL) {
        result = ctx->capacity / ctx->size_item;
    }

    return result;
}


size_t uni_common_array_capacity_bytes(const uni_common_array_t *ctx) {
    size_t result = 0;
    if (ctx != NULL) {
        result = ctx->capacity;
    }

    return result;
}


size_t uni_common_array_size(const uni_common_array_t *ctx) {
    size_t result = 0;
    if (ctx != NULL) {
        result = ctx->size / ctx->size_item;
    }

    return result;
}


size_t uni_common_array_size_bytes(const uni_common_array_t *ctx) {
    size_t result = 0;
    if (ctx != NULL) {
        result = ctx->size;
    }

    return result;
}

size_t uni_common_array_itemsize(const uni_common_array_t *ctx) {
    size_t result = 0;
    if (ctx != NULL) {
        result = ctx->size_item;
    }

    return result;
}


void *uni_common_array_get(uni_common_array_t *ctx, size_t index) {
    void *result = NULL;

    if (ctx != NULL && index < uni_common_array_size(ctx)) {
        result = &ctx->data[ctx->size_item * index];
    }

    return result;
}


bool uni_common_array_set(uni_common_array_t *ctx, size_t index, const void *buf) {
    bool result = false;

    if (ctx != NULL && buf != NULL) {
        void *arr_buf = uni_common_array_get(ctx, index);
        if (arr_buf != NULL) {
            memcpy(arr_buf, buf, ctx->size_item);
            result = true;
        }
    }

    return result;
}


bool uni_common_array_set_itemsize(uni_common_array_t *ctx, size_t item_size) {
    bool result = false;

    if (ctx != NULL && item_size > 0U && item_size <= ctx->capacity) {
        ctx->size_item = item_size;
        result = true;
    }

    return result;
}

bool uni_common_array_push_back(uni_common_array_t *ctx, const void *item) {
    bool result = false;

    if(ctx && item) {
        if(uni_common_array_size(ctx) < uni_common_array_capacity(ctx)) {
            memcpy(&ctx->data[ctx->size], item, ctx->size_item);
            ctx->size += ctx->size_item;
            result = true;
        }
    }

    return result;
}


void *uni_common_array_back(uni_common_array_t *ctx) {
    void *result = NULL;

    if(ctx && uni_common_array_size(ctx) > 0) {
        result = uni_common_array_get(ctx, uni_common_array_size(ctx) - 1);
    }

    return result;
}

void *uni_common_array_front(uni_common_array_t *ctx) {
    void *result = NULL;

    if(ctx) {
        result = uni_common_array_get(ctx, 0);
    }

    return result;
}

bool uni_common_array_reserve(uni_common_array_t *ctx, size_t new_item_count) {
    bool result = false;

    if(ctx) {
        if (new_item_count <= uni_common_array_capacity(ctx))
        {
            ctx->size = new_item_count * ctx->size_item;
            result = true;
        }
    }

    return result;
}

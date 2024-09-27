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

bool uni_common_array_init(uni_common_array_t *ctx, uint8_t *buf, size_t buf_size, size_t item_size) {
    bool result = false;

    if (ctx != NULL && buf != NULL && buf_size > 0U && item_size > 0U) {
        ctx->data = buf;
        ctx->size = buf_size;
        ctx->size_item = item_size;
        result = true; //-V2568
    }

    return result;
}

bool uni_common_array_fill(uni_common_array_t *ctx, uint8_t pattern) {
    bool result = false;

    if (uni_common_array_valid(ctx)) {
        memset(ctx->data, pattern, ctx->size);
        result = true;
    }

    return result;
}

bool uni_common_array_valid(const uni_common_array_t *arr) {
    bool result = false;

    if (arr != NULL && arr->data != NULL && arr->size > 0U && arr->size_item > 0U) {
        result = true;
    }

    return result;
}


uint8_t *uni_common_array_data(uni_common_array_t *ctx) {
    uint8_t *result = NULL;

    if (ctx != NULL) {
        result = ctx->data;
    }

    return result;
}


size_t uni_common_array_length(const uni_common_array_t *ctx) {
    size_t result = 0;
    if (ctx != NULL) {
        result = ctx->size / ctx->size_item;
    }

    return result;
}


size_t uni_common_array_size(const uni_common_array_t *ctx) {
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


uint8_t *uni_common_array_get(uni_common_array_t *ctx, size_t index) {
    uint8_t *result = NULL;

    if (ctx != NULL && index < uni_common_array_length(ctx)) {
        result = &ctx->data[ctx->size_item * index];
    }

    return result;
}


bool uni_common_array_set(uni_common_array_t *ctx, size_t index, const void *buf) {
    bool result = false;

    if (ctx != NULL && buf != NULL) {
        uint8_t *arr_buf = uni_common_array_get(ctx, index);
        if (arr_buf != NULL) {
            memcpy(arr_buf, buf, uni_common_array_itemsize(ctx));
            result = true;
        }
    }

    return result;
}


bool uni_common_array_set_itemsize(uni_common_array_t *ctx, size_t item_size) {
    bool result = false;

    if (ctx != NULL && item_size > 0U && item_size <= ctx->size) {
        ctx->size_item = item_size;
        result = true;
    }

    return result;
}


size_t uni_common_array_pack(uint8_t *out_buf, size_t out_buf_size, const uni_common_array_t *in_arrs, size_t in_arrs_size) {
    size_t data_len = 0;
    size_t data_off = 0;

    if (out_buf != NULL && in_arrs != NULL) {
        for (size_t i = 0; i < in_arrs_size; i++) {
            if (in_arrs[i].data != NULL) {
                data_len += in_arrs[i].size;
            }
        }

        if (data_len <= out_buf_size) {
            for (size_t i = 0; i < in_arrs_size; i++) {
                memcpy(&out_buf[data_off], in_arrs[i].data, in_arrs[i].size);
                data_off += in_arrs[i].size;
            }
        }
    }

    return data_off;
}

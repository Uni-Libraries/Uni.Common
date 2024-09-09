#pragma once

#if defined(__cplusplus)
extern "C" {
#endif


//
// Includes
//

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


//
// Typedefs
//

/**
 * Ringbuffer context structure
 */
typedef struct {
    /**
     * pointer to ringbuffer data array
     */
    uint8_t *data;

    /**
     * size of one object in bytes
     */
    size_t size_object;

    /**
     * Total size of data array in bytes
     *
     * @note size_total % size_object must be == 0
     */
    size_t size_total;

    /**
     * Current front position in bytes
     */
    size_t pos_front;

    /**
     * Current back position in bytes
     */
    size_t pos_back;
} uni_common_ringbuffer_context_t;


//
// Defines
//

#define UNI_COMMON_RINGBUFFER_DEFINITION(name, type, count)   \
type name##_buf[count+1] = {0};                           \
uni_common_ringbuffer_context_t name##_ctx = {                \
    .data = (uint8_t*)name##_buf,                         \
    .size_object = sizeof(type),                          \
    .size_total = sizeof(type)*(count+1),                 \
    .pos_front = 0U,                                      \
    .pos_back = 0U,                                       \
}

#define UNI_COMMON_RINGBUFFER_DECLARATION(name) extern uni_common_ringbuffer_context_t name##_ctx

//
// Functions/Init
//

/**
 * Initializes the ringbuffer
 * @param ctx pointer to the ringbuffer context
 * @param data pointer to the data buffer
 * @param size_object size of one object inside the ringbuffer
 * @param size_total total size of ringbuffer
 * @return true on success
 */
bool uni_common_ringbuffer_init(uni_common_ringbuffer_context_t *ctx, uint8_t *data, uint32_t size_object, uint32_t size_total);


//
// Functions/Getters
//

/**
 * Get element by index
 * @param ctx pointer to the ringbuffer context
 * @param index element index
 * @param data received buffer, must have size of one ringbuffer object
 * @return true on success
 */
bool uni_common_ringbuffer_get(const uni_common_ringbuffer_context_t *ctx, size_t index, uint8_t *data);


/**
 * Check that specified element as present inside the ringbuffer
 * @param ctx pointer to the ringbuffer context
 * @param data pointer to the data buffer to search
 * @param data_len size of the data buffer
 * @return SIZE_MAX if element is not present in ringbuffer, otherwise position of element
 *
 * @note data size must be greater or equal to ctx->size_object
 */
size_t uni_common_ringbuffer_find(const uni_common_ringbuffer_context_t *ctx, const uint8_t *data);


/**
 * Check that ringbuffer is empty
 * @param ctx pointer to the ringbuffer context
 * @return true if ringbuffer is empty
 */
bool uni_common_ringbuffer_is_empty(const uni_common_ringbuffer_context_t *ctx);


/**
 * Check that ringbuffer is full
 * @param ctx pointer to the ringbuffer context
 * @return true if ringbuffer is full
 */
bool uni_common_ringbuffer_is_full(const uni_common_ringbuffer_context_t *ctx);


/**
 * Number of remaining (used) elements in ringbuffer
 * @param ctx pointer to the ringbuffer context
 * @return number of stored elements
 */
size_t uni_common_ringbuffer_length(const uni_common_ringbuffer_context_t *ctx);


//
// Functions/Operations
//

/**
 * Clears the ringbuffer
 * @param ctx pointer to the ringbuffer context
 * @return true on success
 */
bool uni_common_ringbuffer_clear(uni_common_ringbuffer_context_t *ctx);


/**
 * Pops specified number of objects from ringbuffer
 * @param ctx pointer to the ringbuffer context
 * @param data receive buffer, must be >= count * ctx->size_object
 * @param count number of objects to pop
 * @return number of returned objects objects
 */
size_t uni_common_ringbuffer_pop(uni_common_ringbuffer_context_t *ctx, uint8_t *data, size_t count);

/**
 * Pushes one object from ringbuffer
 * @param ctx pointer to the ringbuffer context
 * @param data pointer to the send buffer
 * @param count number of objects to push
 * @return number of pushed objects
 *
 * @note size of buffer must be greater or equal to ctx->size_object
 */
size_t uni_common_ringbuffer_push(uni_common_ringbuffer_context_t *ctx, const uint8_t *data, size_t count);


#if defined(__cplusplus)
}
#endif

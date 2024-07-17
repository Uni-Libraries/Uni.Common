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
// Defines
//

#define UNIMCU_ARRAY_DEFINITION(name)                  \
unimcu_array_t name##_ctx = {                          \
   .data = (uint8_t*)name##_buf,                       \
   .size = sizeof(name##_buf),                         \
   .size_item = sizeof(name##_buf[0]),                 \
}                                                      \


#define UNIMCU_ARRAY_DEFINITION_EX(name, type, count)  \
type name##_buf[count] = {0};                          \
UNIMCU_ARRAY_DEFINITION(name)                          \


#define UNIMCU_ARRAY_DECLARATION(name, type, count)    \
extern type name##_buf[count];                         \
extern unimcu_array_t name##_ctx                       \



//
// Typedefs
//

/**
 * Array context structure
 */
typedef struct {
    /**
     * Pointer to the data buffer
     */
    uint8_t *data;

    /**
     * Size of the data buffer
     */
    size_t size;

    /**
     * Size of one array element in data buffer
     */
     size_t size_item;
} unimcu_array_t;


//
// Functions
//

/**
 * Initializes array
 * @param ctx pointer to the array context
 * @param buf pointer to the data buffer
 * @param buf_size size of data buffer
 * @param item_size size of one array element
 * @return true on success
 */
bool unimcu_array_init(unimcu_array_t *ctx, uint8_t *buf, size_t buf_size, size_t item_size);


/**
 * Fill array with the following pattern
 * @param ctx pointer to the array context
 * @param pattern pattern to fill
 * @return true on success
 */
bool unimcu_array_fill(unimcu_array_t *ctx, uint8_t pattern);


/**
 * Checks that array is valid
 * @param arr pointer to the array context
 * @return true if array is valid
 */
bool unimcu_array_valid(const unimcu_array_t *arr);


/**
 * Returns pointer to the array data buffer
 * @param сеч pointer to the array context
 * @return pointer to the data
 */
uint8_t *unimcu_array_data(unimcu_array_t *ctx);


/**
 * Returns length of array in elements
 * @param arr pointer to the array context
 * @return size of array in elements
 */
size_t unimcu_array_length(const unimcu_array_t *ctx);


/**
 * Returns length of array in bytes
 * @param arr pointer to the array context
 * @return size of array in bytes
 */
size_t unimcu_array_size(const unimcu_array_t *ctx);


/**
 * Returns length of one element in bytes
 * @param arr pointer to the array context
 * @return size of one item in bytes
 */
size_t unimcu_array_itemsize(const unimcu_array_t *ctx);

/**
 * Receive array element via its index
 * @param ctx pointer to the array context
 * @param index element index
 * @return pointer to the start of element
 */
uint8_t *unimcu_array_get(unimcu_array_t *ctx, size_t index);


/**
 * Sets array element via its index
 * @param ctx pointer to the array context
 * @param index element index
 * @param buf buffer which contains array element
 * @note input array MUST be equal or greater than array element isze
 * @return true on success
 */
bool unimcu_array_set(unimcu_array_t *ctx, size_t index, const void *buf);


/**
 * Sets array item size
 * @param ctx pointer to the array context
 * @param item_size size of one element in bytes
 * @return true on success
 */
bool unimcu_array_set_itemsize(unimcu_array_t *ctx, size_t item_size);


/**
 * Merges several of inputs arrays into the output buffer
 * @param out_buf pointer to the output buffer
 * @param out_buf_size output buffer size
 * @param in_arrs pointer to the array of input arrays to be merges
 * @param in_arrs_size count of input arrays
 * @return size of merged array
 */
size_t unimcu_array_pack(uint8_t *out_buf, size_t out_buf_size, const unimcu_array_t * in_arrs, size_t in_arrs_size);

#if defined(__cplusplus)
}
#endif

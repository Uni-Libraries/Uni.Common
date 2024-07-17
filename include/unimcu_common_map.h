#pragma once

/**
 * Map implementation
 *
 * data types:
 *   * key is size_t
 *   * value is user-defined variable or struct
 */

#if defined(__cplusplus)
extern "C" {
#endif

//
// Includes
//

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "unimcu_common_array.h"



//
// Deifnes
//

#define unimcu_map_DEFINITION(name, type, count)                                    \
unimcu_ARRAY_DEFINITION(name##_arr_link_prev, size_t, count);                          \
unimcu_ARRAY_DEFINITION(name##_arr_link_next, size_t, count);                          \
unimcu_ARRAY_DEFINITION(name##_arr_keys     , size_t, count);                          \
unimcu_ARRAY_DEFINITION(name##_arr_vals     , type  , count);                          \
unimcu_map_context_t name##_ctx = {                                                 \
    .arr_link_prev = &name##_arr_link_prev_ctx,                                        \
    .arr_link_next = &name##_arr_link_next_ctx,                                        \
    .arr_keys = &name##_arr_keys_ctx,                                                  \
    .arr_vals = &name##_arr_vals_ctx,                                                  \
    .slot_first = SIZE_MAX,                                                            \
    .slot_last = SIZE_MAX                                                              \
}unimcu_common_map

#define unimcu_map_DECLARATION(name, type, count)                                   \
unimcu_ARRAY_DECLARATION(name##_arr_keys     , size_t, count);                         \
unimcu_ARRAY_DECLARATION(name##_arr_vals     , type, count);                           \
extern unimcu_map_context_t name##_ctx



//
// Typedefs
//

/**
 * Typedef for enumerator function
 *
 * @param key LRU-map item key
 * @param val pointer tot the LRU-map item value
 */
typedef void (*unimcu_map_enum_func_t)(size_t key, const void *val);

typedef struct {
    /**
     * Pointer to the LRU-map keys array
     */
    unimcu_array_t *keys;

    /**
     * Pointer to the LRU-map values array
     */
     unimcu_array_t *vals;
} unimcu_map_config_t;


/**
 * LRU-map context structure
 */
typedef struct {
    size_t size;

    size_t capacity;

    /**
     * Flags which stores the initialization state
     */
    bool initialized;

} unimcu_map_state_t;


typedef struct {
  unimcu_map_config_t config;
  unimcu_map_state_t state;
} unimcu_map_context_t;



//
// Functions/Init
//

/**
 * Initializes map
 * @param ctx pointer to the LRU map context
 * @param arr_keys pointer to the array of map keys
 * @param arr_vals pointer to the array of map values
 * @note :arr_keys element size will be changed to sizeof(size_t)
 * @note LRU-map slot count is min(arr_keys.length(), arr_values.length())
 * @return true on success
 */
bool unimcu_map_init(unimcu_map_context_t *ctx, unimcu_array_t *arr_keys, unimcu_array_t *arr_vals);



//
// Functions/Getter
//

/**
 * Returns LRU-map capacity
 * @param ctx pointer to the LRU-map
 * @return count of possible unique keys in LRU-map
 */
size_t unimcu_map_capacity(const unimcu_map_context_t *ctx);


/**
 * Checks that map was initialized
 * @param ctx pointer to the LRU-map context
 * @return true if map was properly initialized
 */
bool unimcu_map_initialized(const unimcu_map_context_t *ctx);


/**
 * Returns count of used LRU-map slots
 * @param ctx pointer to the LRU-map
 * @return numbe of used slots
 *
 * @note use :unimcu_map_capacity to get total number of slots
 */
size_t unimcu_map_size(const unimcu_map_context_t *ctx);



//
// Functions/Process
//

/**
 * Resets LRU-map to the initial state
 * @param ctx pointer to the LRU-map
 * @return true on success
 */
bool unimcu_map_clear(unimcu_map_context_t *ctx);


/**
 * Enumerates LRU-map
 * @param ctx pointer to the LRU-map context
 * @param func pointer to the enumerator function
 * @return true on success
 */
bool unimcu_map_enum(unimcu_map_context_t *ctx, unimcu_map_enum_func_t func);


/**
 * Get pointer to the start of map element value by element key
 * @param ctx pointer to the LRU-map context
 * @param key map item key
 * @return pointer to the element value, NULL if element does not exists
 */
uint8_t *unimcu_map_get(unimcu_map_context_t *ctx, size_t key);


/**
 * Removes element with the given key from the LRU-map
 * @param ctx pointer to the LRU-map context
 * @param key key to remove
 * @return true on sucess (element was removed)
 */
bool unimcu_map_remove(unimcu_map_context_t *ctx, size_t key);


/**
 * Updates the content inside the map for the given key
 * @param ctx pointer to the LRU-map content
 * @param key element key
 * @param val pointer to the element value
 * @return true on success
 */
bool unimcu_map_set(unimcu_map_context_t *ctx, size_t key, const void *val);


#if defined(__cplusplus)
}
#endif

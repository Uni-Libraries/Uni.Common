#pragma once

/**
 * LRU(least recently _updated_)-map implementation
 *
 * behavior:
 *  * when full, it replaces the element which was least recently updated with a new one.
 *  * the oldest element is the first one in the list, the newest is the last
 *
 * data storage:
 *   * it uses double-linked list due to static memory allocation requirement
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

#include "uni_common_array.h"



//
// Deifnes
//

#define uni_common_LRUMAP_DEFINITION(name, type, count)                                    \
uni_common_ARRAY_DEFINITION(name##_arr_link_prev, size_t, count);                          \
uni_common_ARRAY_DEFINITION(name##_arr_link_next, size_t, count);                          \
uni_common_ARRAY_DEFINITION(name##_arr_keys     , size_t, count);                          \
uni_common_ARRAY_DEFINITION(name##_arr_vals     , type  , count);                          \
uni_common_lrumap_context_t name##_ctx = {                                                 \
    .arr_link_prev = &name##_arr_link_prev_ctx,                                        \
    .arr_link_next = &name##_arr_link_next_ctx,                                        \
    .arr_keys = &name##_arr_keys_ctx,                                                  \
    .arr_vals = &name##_arr_vals_ctx,                                                  \
    .slot_first = SIZE_MAX,                                                            \
    .slot_last = SIZE_MAX                                                              \
}

#define uni_common_LRUMAP_DECLARATION(name, type, count)                                   \
uni_common_ARRAY_DECLARATION(name##_arr_link_prev, size_t, count);                         \
uni_common_ARRAY_DECLARATION(name##_arr_link_next, size_t, count);                         \
uni_common_ARRAY_DECLARATION(name##_arr_keys     , size_t, count);                         \
uni_common_ARRAY_DECLARATION(name##_arr_vals     , type, count);                           \
extern uni_common_lrumap_context_t name##_ctx



//
// Typedefs
//

/**
 * Typedef for enumerator function
 *
 * @param key LRU-map item key
 * @param val pointer tot the LRU-map item value
 */
typedef void (*uni_common_lrumap_enum_func_t)(size_t key, const void *val);

/**
 * LRU-map context structure
 */
typedef struct {
    /**
     * Index of first array slot in list
     * @note equal to SIZE_MAX in case of empty list
     */
    size_t slot_first;

    /**
     * Index of last array slot in list
     * @note equal to SIZE_MAX in case of empty list
     */
    size_t slot_last;

    /**
     * Pointer to the link-to-previous list linkage array
     */
    uni_common_array_t *arr_link_prev;

    /**
     * Pointer to the link-to-next list linkage array
     */
    uni_common_array_t *arr_link_next;

    /**
     * Pointer to the LRU-map keys array
     */
    uni_common_array_t *arr_keys;

    /**
     * Pointer to the LRU-map values array
     */
    uni_common_array_t *arr_vals;


    /**
     * Flags which stores the initialization state
     */
    bool initialized;
} uni_common_lrumap_context_t;


//
// Functions/Init
//

/**
 * Initializes LRU map
 * @param ctx pointer to the LRU map context
 * @param arr_link_prev pointer to the link-to-previous list linkage array
 * @param arr_link_next pointer to the link-to-next list linkage array
 * @param arr_keys pointer to the array of map keys
 * @param arr_vals pointer to the array of map values
 * @note :arr_link_prev, :arr_link_next, :arr_keys element size will be changed to sizeof(size_t)
 * @note LRU-map slot count is min(arr_link_prev.length(), arr_link_next.length(), arr_keys.length(),
 * arr_values.length())
 * @return true on success
 */
bool uni_common_lrumap_init(uni_common_lrumap_context_t *ctx, uni_common_array_t *arr_link_prev, uni_common_array_t *arr_link_next,
                        uni_common_array_t *arr_keys, uni_common_array_t *arr_vals);


//
// Functions/Getter
//


/**
 * Returns LRU-map capacity
 * @param ctx pointer to the LRU-map
 * @return count of possible unique keys in LRU-map
 */
size_t uni_common_lrumap_capacity(const uni_common_lrumap_context_t *ctx);


/**
 * Checks that LRU-map is empty
 * @param ctx pointer to the LRU-map context
 * @return true if LRU-map is empty
 */
bool uni_common_lrumap_empty(const uni_common_lrumap_context_t *ctx);


/**
 * Checks that LRUmap was initialized
 * @param ctx pointer to the LRU-map context
 * @return true if map was properly initialized
 */
bool uni_common_lrumap_initialized(const uni_common_lrumap_context_t *ctx);


/**
 * Returns count of used LRU-map slots
 * @param ctx pointer to the LRU-map
 * @return numbe of used slots
 *
 * @note use :uni_common_lrumap_capacity to get total number of slots
 */
size_t uni_common_lrumap_length(const uni_common_lrumap_context_t *ctx);


//
// Functions/Process
//

/**
 * Resets LRU-map to the initial state
 * @param ctx pointer to the LRU-map
 * @return true on success
 */
bool uni_common_lrumap_clear(uni_common_lrumap_context_t *ctx);

/**
 * Enumerates LRU-map
 * @param ctx pointer to the LRU-map context
 * @param func pointer to the enumerator function
 * @return true on success
 */
bool uni_common_lrumap_enum(uni_common_lrumap_context_t *ctx, uni_common_lrumap_enum_func_t func);

/**
 * Get pointer to the start of map element value by element key
 * @param ctx pointer to the LRU-map context
 * @param key map item key
 * @return pointer to the element value, NULL if element does not exists
 */
uint8_t *uni_common_lrumap_get(uni_common_lrumap_context_t *ctx, size_t key);


/**
 * Ger pointer to the value of the first element of LRU-map
 * @param ctx pointer to the LRU-map context
 * @return NULL on non-existent element
 */
uint8_t *uni_common_lrumap_get_first(uni_common_lrumap_context_t *ctx);


/**
 * Ger pointer to the value of the last element of LRU-map
 * @param ctx pointer to the LRU-map context
 * @return NULL on non-existent element
 */
uint8_t *uni_common_lrumap_get_last(uni_common_lrumap_context_t *ctx);


/**
 * Get content of specific list index inside the map
 * @param ctx pointer to the LRU-map context
 * @param idx idx to get
 * @param key pointer which will contain key content
 * @param val pointer which will contain cal content
 * @return true on success
 */
bool uni_common_lrumap_get_idx(uni_common_lrumap_context_t *ctx, size_t idx, size_t *key, void *val);


/**
 * Removes element with the given key from the LRU-map
 * @param ctx pointer to the LRU-map context
 * @param key key to remove
 * @return true on sucess (element was removed)
 */
bool uni_common_lrumap_remove(uni_common_lrumap_context_t *ctx, size_t key);


/**
 * Remove the first element of LRU-map
 * @param ctx pointer to the LRU-map context
 * @return true on sucess (element was removed)
 */
bool uni_common_lrumap_remove_first(uni_common_lrumap_context_t *ctx);


/**
 * Remove the last element of LRU-map
 * @param ctx pointer to the LRU-map context
 * @return true on sucess (element was removed)
 */
bool uni_common_lrumap_remove_last(uni_common_lrumap_context_t *ctx);


/**
 * Updates the content inside the map for the given key
 * @param ctx pointer to the LRU-map content
 * @param key element key
 * @param val pointer to the element value
 * @return true on success
 */
bool uni_common_lrumap_update(uni_common_lrumap_context_t *ctx, size_t key, const void *val);


#if defined(__cplusplus)
}
#endif

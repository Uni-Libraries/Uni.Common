#pragma once

//
// Includes
//

#include <stdbool.h>
#include <stddef.h>



//
// Typedefs
//

typedef struct {
    const char* delims;

    char*  str;
    size_t str_len;

    char* tok_start;
} unimcu_tokenizer_context_t;



//
// Functions
//

bool unimcu_tokenizer_init(unimcu_tokenizer_context_t* ctx, char* str, const char* delimiters, size_t str_len);

char* unimcu_tokenizer_next(unimcu_tokenizer_context_t* ctx);

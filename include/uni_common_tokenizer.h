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
} uni_common_tokenizer_context_t;



//
// Functions
//

bool uni_common_tokenizer_init(uni_common_tokenizer_context_t* ctx, char* str, const char* delimiters, size_t str_len);

char* uni_common_tokenizer_next(uni_common_tokenizer_context_t* ctx);

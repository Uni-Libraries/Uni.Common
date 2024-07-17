//
// Includes
//

#include <stddef.h>
#include <string.h>

#include "unimcu_common_tokenizer.h"



//
// Tokenizer
//

bool unimcu_tokenizer_init(unimcu_tokenizer_context_t* ctx, char* str, const char* delimiters, size_t str_len){
    bool result = false;

    if(ctx != NULL && str != NULL && delimiters != NULL) {
        ctx->str = str;
        ctx->str_len = str_len;
        ctx->delims = delimiters;

        ctx->tok_start = str;

        if(ctx->str_len == 0) {
            ctx->str_len = strlen(str);
        }
    }

    return result;
}


char* unimcu_tokenizer_next(unimcu_tokenizer_context_t* ctx){
    char* result = NULL;

    if (ctx != NULL && ctx->tok_start != NULL) {
        result = ctx->tok_start;

        char *delim_addr = strpbrk(ctx->tok_start, ctx->delims);
        if (delim_addr != NULL) {
            size_t delim_pos = delim_addr - ctx->str;
            ctx->str[delim_pos] = '\0';
            if (delim_pos + 1U < ctx->str_len) {
                ctx->tok_start = &ctx->str[delim_pos + 1U];
            } else {
                ctx->tok_start = NULL;
            }
        } else {
            ctx->tok_start = NULL;
        }
    }

    return result;
}

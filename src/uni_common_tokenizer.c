//
// Includes
//

#include <stddef.h>
#include <string.h>

#include "uni_common_tokenizer.h"



//
// Tokenizer
//

bool uni_common_tokenizer_init(uni_common_tokenizer_context_t* ctx, char* str, const char* delimiters, size_t str_len){
    bool result = false;

    if(ctx != NULL && str != NULL && delimiters != NULL) {
        ctx->str = str;
        ctx->str_len = str_len;
        ctx->delims = delimiters;

        ctx->tok_start = str;

        if(ctx->str_len == 0) {
            ctx->str_len = strlen(str);
        }

        result = true;
    }

    return result;
}


char* uni_common_tokenizer_next(uni_common_tokenizer_context_t* ctx){
    char* result = NULL;

    if (ctx != NULL) {
        while (ctx->tok_start != NULL) {
            size_t start_idx = (size_t)(ctx->tok_start - ctx->str);

            if (start_idx >= ctx->str_len) {
                ctx->tok_start = NULL;
                break;
            }

            // Length-bounded scan for any delimiter within the remaining buffer
            size_t i = start_idx;
            char* delim_addr = NULL;
            for (; i < ctx->str_len; ++i) {
                if (strchr(ctx->delims, ctx->str[i]) != NULL) {
                    delim_addr = &ctx->str[i];
                    break;
                }
            }

            result = &ctx->str[start_idx];

            if (delim_addr != NULL) {
                size_t delim_pos = (size_t)(delim_addr - ctx->str);
                ctx->str[delim_pos] = '\0';
                if (delim_pos + 1U < ctx->str_len) {
                    ctx->tok_start = &ctx->str[delim_pos + 1U];
                } else {
                    ctx->tok_start = NULL;
                }
            } else {
                // No delimiter within bounds: last token spans to the end of provided length
                ctx->tok_start = NULL;
            }

            if (*result != '\0') {
                break;
            } else {
                result = NULL;
            }
        }
    }

    return result;
}

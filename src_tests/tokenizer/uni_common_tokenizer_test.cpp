//
// Includes
//

// catch2
#include <catch2/catch_test_macros.hpp>

// uni.common
#include "uni_common_tokenizer.h"



//
// tests
//

TEST_CASE("tokenizer_init", "[tokenizer]"){
    SECTION("null_string") {
        uni_common_tokenizer_context_t tok{};
        REQUIRE_FALSE(uni_common_tokenizer_init(&tok, nullptr, " ", 10));
    }
}


TEST_CASE("tokenizer_ftp_filelist", "[tokenizer]") {
    const char* source = 
"-rw-r--r--    1 1000     1000     16754599 Mar 06 23:05 meow.bin\r\n-rw-r--r--    1 1000     1000     15105798 Mar 06 23:04 gaw.bin\r\n-rw-r--r--    1 1000     1000     14500631 Mar 06 23:04 moo.bin\r\n";
    const char* words[3] = {
        "-rw-r--r--    1 1000     1000     16754599 Mar 06 23:05 meow.bin",
        "-rw-r--r--    1 1000     1000     15105798 Mar 06 23:04 gaw.bin",
        "-rw-r--r--    1 1000     1000     14500631 Mar 06 23:04 moo.bin"
    };

    std::vector<char> b(strlen(source) + 1);
    memcpy(b.data(), source, b.size());

    uni_common_tokenizer_context_t tok_1;
    REQUIRE(uni_common_tokenizer_init(&tok_1, b.data(), "\r\n", b.size()));

    char* word = nullptr;
    int count = 0;
    while(word = uni_common_tokenizer_next(&tok_1)){
        REQUIRE(strcmp(word, words[count])==0);
        count++;
    }
    REQUIRE(count == 3);
}

TEST_CASE("tokenizer_edge_cases", "[tokenizer]") {
    SECTION("only_delimiters") {
        const char* source = "\r\n\r\n";
        std::vector<char> b(strlen(source) + 1);
        memcpy(b.data(), source, b.size());
        uni_common_tokenizer_context_t tok;
        REQUIRE(uni_common_tokenizer_init(&tok, b.data(), "\r\n", b.size()));
        REQUIRE(uni_common_tokenizer_next(&tok) == nullptr);
    }

    SECTION("leading_delimiters") {
        const char* source = "\r\nword";
        const char* words[] = { "word" };
        std::vector<char> b(strlen(source) + 1);
        memcpy(b.data(), source, b.size());
        uni_common_tokenizer_context_t tok;
        REQUIRE(uni_common_tokenizer_init(&tok, b.data(), "\r\n", b.size()));
        
        char* word = nullptr;
        int count = 0;
        while(word = uni_common_tokenizer_next(&tok)){
            REQUIRE(strcmp(word, words[count])==0);
            count++;
        }
        REQUIRE(count == 1);
    }

    SECTION("trailing_delimiters") {
        const char* source = "word\r\n";
        const char* words[] = { "word" };
        std::vector<char> b(strlen(source) + 1);
        memcpy(b.data(), source, b.size());
        uni_common_tokenizer_context_t tok;
        REQUIRE(uni_common_tokenizer_init(&tok, b.data(), "\r\n", b.size()));
        
        char* word = nullptr;
        int count = 0;
        while(word = uni_common_tokenizer_next(&tok)){
            REQUIRE(strcmp(word, words[count])==0);
            count++;
        }
        REQUIRE(count == 1);
    }

    SECTION("no_delimiters") {
        const char* source = "word";
        const char* words[] = { "word" };
        std::vector<char> b(strlen(source) + 1);
        memcpy(b.data(), source, b.size());
        uni_common_tokenizer_context_t tok;
        REQUIRE(uni_common_tokenizer_init(&tok, b.data(), ";", b.size()));
        
        char* word = nullptr;
        int count = 0;
        while(word = uni_common_tokenizer_next(&tok)){
            REQUIRE(strcmp(word, words[count])==0);
            count++;
        }
        REQUIRE(count == 1);
    }
}

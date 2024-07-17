cmake_minimum_required(VERSION 3.21)

project(uni.common)

add_library(uni.common STATIC)

target_include_directories(uni.common PUBLIC "include")

target_sources(uni.common PRIVATE
    "src/uni_common_array.c"
    "src/uni_common_bytes.c"
    "src/uni_common_libc.c"
    "src/uni_common_lrumap.c"
    "src/uni_common_map.c"
    "src/uni_common_ringbuffer.c"
    "src/uni_common_tokenizer.c"
)

if(MSVC)
    target_compile_features(uni.common PRIVATE c_std_17)
else()
    target_compile_features(uni.common PRIVATE c_std_23)
endif()

if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(src_tests)
endif()

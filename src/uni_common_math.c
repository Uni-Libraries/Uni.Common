#include "uni_common_math.h"

uint16_t uni_common_math_map_u32_u16(uint32_t input, uint32_t input_min, uint16_t output_min, uint32_t input_max, uint16_t output_max) {
    uint16_t result = UINT16_MAX;

    // clamping and validation
    if (input_min == input_max) {
        result = output_min;
    }
    else if (input <= input_min) {
        result = output_min;
    }
    else if (input >= input_max) {
        result = output_max;
    }
    else {
        uint32_t input_range = input_max - input_min;
        uint32_t input_delta = input - input_min;
        if (output_min <= output_max) {
            uint32_t output_range = (uint32_t)output_max - output_min;
            result = (uint16_t)(output_min + ((uint64_t)input_delta * output_range + input_range / 2u) / input_range);
        } else {
            uint32_t output_range = (uint32_t)output_min - output_max;
            result = (uint16_t)(output_min - ((uint64_t)input_delta * output_range + input_range / 2u) / input_range);
        }
    }

    return result;
}

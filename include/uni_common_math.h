#pragma once



//
// Defines
//

/**
 * Returns the maximum value between two numbers
 */
#define uni_common_math_max(a, b) ((a)>(b) ? (a) : (b))

/**
 * Returns the maximum value between three numbers
 */
#define uni_common_math_max3(a, b, c) uni_common_math_max(uni_common_math_max(a,b),c)

/**
 * Returns the minimum value between two numbers
 */
#define uni_common_math_min(a, b) ((a)<(b) ? (a) : (b))

/**
 * Returns the minimum value between three numbers
 */
#define uni_common_math_min3(a, b, c) uni_common_math_min(uni_common_math_min(a,b),c)

/**
 * Clamps number between minimum and maximum values
 */
#define uni_common_math_clamp(a, min, max) (uni_common_math_min(uni_common_math_max((a), (min)),(max)))

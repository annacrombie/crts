#ifndef SHARED_PATHFIND_MACROS_H
#define SHARED_PATHFIND_MACROS_H

/* helpers for efficent getting/setting of a sub-byte array of "arbitrary"
 * granularity.  usage of shifts and masks rather than division and modulo
 * might be overkill :) */

#define SUB_BYTE_GET(es, div, mod, mul, mask, i) ((es[(i) >> (div)] >> (((i) & mod) * mul)) & mask)
#define SUB_BYTE_SET(es, div, mod, mul, i, v) (es[(i) >> (div)] |= (v << (((i) & mod) * mul)))

/* indexes 1-bit chunks */
#define SB1_GET(es, i) SUB_BYTE_GET(es, 3, 7, 1, 1, i)
#define SB1_SET(es, i, v) SUB_BYTE_SET(es, 3, 7, 1, i, v)

/* indexes 4-bit chunks */
#define SB4_GET(es, i) SUB_BYTE_GET(es, 1, 1, 4, 0xf, i)
#define SB4_SET(es, i, v) SUB_BYTE_SET(es, 1, 1, 4, i, v)

/* getters for adjacent x-col indices */
#define LEFT_OF(i) ((i) - 16)
#define RIGHT_OF(i) ((i) + 16)
#define ABOVE(i) ((i) - 1)
#define BELOW(i) ((i) + 1)

#define TRAV_LEFT_OF(trav, i) (i > 15 ? (SB1_GET(trav, LEFT_OF(i))) : 0)
#define TRAV_RIGHT_OF(trav, i) (i < 240 ? (SB1_GET(trav, RIGHT_OF(i))) : 0)
#define TRAV_ABOVE(trav, i) (i & 15 ? (SB1_GET(trav, ABOVE(i))) : 0)
#define TRAV_BELOW(trav, i) ((i + 1) & 15 ? (SB1_GET(trav, BELOW(i))) : 0)

/*
   _0,  16,  32,  48,  64,  80,  96, 112, 128, 144, 160, 176, 192, 208, 224, 240,
   _1,  17,  33,  49,  65,  81,  97, 113, 129, 145, 161, 177, 193, 209, 225, 241,
   _2,  18,  34,  50,  66,  82,  98, 114, 130, 146, 162, 178, 194, 210, 226, 242,
   _3,  19,  35,  51,  67,  83,  99, 115, 131, 147, 163, 179, 195, 211, 227, 243,
   _4,  20,  36,  52,  68,  84, 100, 116, 132, 148, 164, 180, 196, 212, 228, 244,
   _5,  21,  37,  53,  69,  85, 101, 117, 133, 149, 165, 181, 197, 213, 229, 245,
   _6,  22,  38,  54,  70,  86, 102, 118, 134, 150, 166, 182, 198, 214, 230, 246,
   _7,  23,  39,  55,  71,  87, 103, 119, 135, 151, 167, 183, 199, 215, 231, 247,
   _8,  24,  40,  56,  72,  88, 104, 120, 136, 152, 168, 184, 200, 216, 232, 248,
   _9,  25,  41,  57,  73,  89, 105, 121, 137, 153, 169, 185, 201, 217, 233, 249,
   10,  26,  42,  58,  74,  90, 106, 122, 138, 154, 170, 186, 202, 218, 234, 250,
   11,  27,  43,  59,  75,  91, 107, 123, 139, 155, 171, 187, 203, 219, 235, 251,
   12,  28,  44,  60,  76,  92, 108, 124, 140, 156, 172, 188, 204, 220, 236, 252,
   13,  29,  45,  61,  77,  93, 109, 125, 141, 157, 173, 189, 205, 221, 237, 253,
   14,  30,  46,  62,  78,  94, 110, 126, 142, 158, 174, 190, 206, 222, 238, 254,
   15,  31,  47,  63,  79,  95, 111, 127, 143, 159, 175, 191, 207, 223, 239, 255,

   48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
   _0,                                                                        32,
   _1,                                                                        33,
   _2,                                                                        34,
   _3,                                                                        35,
   _4,                                                                        36,
   _5,                                                                        37,
   _6,                                                                        38,
   _7,                                                                        39,
   _8,                                                                        40,
   _9,                                                                        41,
   10,                                                                        42,
   11,                                                                        43,
   12,                                                                        44,
   13,                                                                        45,
   14,                                                                        46,
   15,                                                                        47,
   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
 */

#endif

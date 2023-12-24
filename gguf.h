/* This code is adapted from https://github.com/ggerganov/ggml/
 * The changes are copyright (C) 2024 Salvatore Sanfilippo <antirez@gmail.com>
 * See LICENSE for licensing info. */

#include <stdint.h>

enum gguf_tensor_type {
    GUFF_TYPE_F32  = 0,
    GUFF_TYPE_F16  = 1,
    GUFF_TYPE_Q4_0 = 2,
    GUFF_TYPE_Q4_1 = 3,
    // GUFF_TYPE_Q4_2 = 4, support has been removed
    // GUFF_TYPE_Q4_3 (5) support has been removed
    GUFF_TYPE_Q5_0 = 6,
    GUFF_TYPE_Q5_1 = 7,
    GUFF_TYPE_Q8_0 = 8,
    GUFF_TYPE_Q8_1 = 9,
    // k-quantizations
    GUFF_TYPE_Q2_K = 10,
    GUFF_TYPE_Q3_K = 11,
    GUFF_TYPE_Q4_K = 12,
    GUFF_TYPE_Q5_K = 13,
    GUFF_TYPE_Q6_K = 14,
    GUFF_TYPE_Q8_K = 15,
    GUFF_TYPE_I8,
    GUFF_TYPE_I16,
    GUFF_TYPE_I32,
    GUFF_TYPE_COUNT,
};

enum gguf_value_type {
    // The value is a 8-bit unsigned integer.
    GGUF_VALUE_TYPE_UINT8 = 0,
    // The value is a 8-bit signed integer.
    GGUF_VALUE_TYPE_INT8 = 1,
    // The value is a 16-bit unsigned little-endian integer.
    GGUF_VALUE_TYPE_UINT16 = 2,
    // The value is a 16-bit signed little-endian integer.
    GGUF_VALUE_TYPE_INT16 = 3,
    // The value is a 32-bit unsigned little-endian integer.
    GGUF_VALUE_TYPE_UINT32 = 4,
    // The value is a 32-bit signed little-endian integer.
    GGUF_VALUE_TYPE_INT32 = 5,
    // The value is a 32-bit IEEE754 floating point number.
    GGUF_VALUE_TYPE_FLOAT32 = 6,
    // The value is a boolean.
    // 1-byte value where 0 is false and 1 is true.
    // Anything else is invalid, and should be treated as either the model
    // being invalid or the reader being buggy.
    GGUF_VALUE_TYPE_BOOL = 7,
    // The value is a UTF-8 non-null-terminated string, with length prepended.
    GGUF_VALUE_TYPE_STRING = 8,
    // The value is an array of other values, with the length and type
    // prepended. Arrays can be nested, and the length of the array is the
    // number of elements in the array, not the number of bytes.
    GGUF_VALUE_TYPE_ARRAY = 9,
    // The value is a 64-bit unsigned little-endian integer.
    GGUF_VALUE_TYPE_UINT64 = 10,
    // The value is a 64-bit signed little-endian integer.
    GGUF_VALUE_TYPE_INT64 = 11,
    // The value is a 64-bit IEEE754 floating point number.
    GGUF_VALUE_TYPE_FLOAT64 = 12,
    // Special values used by the callbacks of gguf_do_with_value().
    GGUF_VALUE_TYPE_ARRAY_START = 100,
    GGUF_VALUE_TYPE_ARRAY_END = 101
};

const char *gguf_value_name[] = {
    "uint8", "int8", "uint16", "int16", "uint32", "int32",
    "float32", "bool", "string", "array", "uint64", "int64",
    "float64"
};

// A string in GGUF.
struct gguf_string {
    // The length of the string, in bytes.
    uint64_t len;
    // The string as a UTF-8 non-null-terminated string.
    char string[];
};

// Union of possible values.
union gguf_value {
    uint8_t uint8;
    int8_t int8;
    uint16_t uint16;
    int16_t int16;
    uint32_t uint32;
    int32_t int32;
    float float32;
    uint64_t uint64;
    int64_t int64;
    double float64;
    uint8_t boolval;
    struct gguf_string string;
    struct {
        // Any value type is valid, including arrays.
        uint32_t type;
        // Number of elements, not bytes
        uint64_t len;
        // The array of values follow...
    } __attribute__((packed)) array;
};

// Header
struct gguf_header {
    // Magic number to announce that this is a GGUF file.
    // Must be `GGUF` at the byte level: `0x47` `0x47` `0x55` `0x46`.
    uint32_t magic;
    // The version of the format implemented.
    // Must be `3` for version described in this spec.
    uint32_t version;
    // The number of tensors in the file.
    // This is explicit, instead of being included in the metadata, to ensure
    // it is always present for loading the tensors.
    uint64_t tensor_count;
    // The number of metadata key-value pairs.
    uint64_t metadata_kv_count;
};

/* Key represneation in this library API. */
typedef struct {
    const char *name;
    size_t namelen;
    uint32_t type;
    union gguf_value *val;
} gguf_key;

typedef struct {
    const char *name;
    size_t namelen;
    int type;
} gguf_value;
#ifndef CDICT_H
#define CDICT_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* xxhash algorithm */

typedef uint64_t cdict__XXH64_hash_t;
typedef uint8_t cdict__xxh_u8;

typedef cdict__XXH64_hash_t cdict__xxh_u64;

#define cdict__XXH_FORCE_INLINE                                                \
  static __inline__ __attribute__((always_inline, unused))

#define cdict__XXH_PRIME64_1                                                                  \
  0x9E3779B185EBCA87ULL /*!<                                                                  \
                           0b1001111000110111011110011011000110000101111010111100101010000111 \
                         */
#define cdict__XXH_PRIME64_2                                                                  \
  0xC2B2AE3D27D4EB4FULL /*!<                                                                  \
                           0b1100001010110010101011100011110100100111110101001110101101001111 \
                         */
#define cdict__XXH_PRIME64_3                                                                  \
  0x165667B19E3779F9ULL /*!<                                                                  \
                           0b0001011001010110011001111011000110011110001101110111100111111001 \
                         */
#define cdict__XXH_PRIME64_4                                                                  \
  0x85EBCA77C2B2AE63ULL /*!<                                                                  \
                           0b1000010111101011110010100111011111000010101100101010111001100011 \
                         */
#define cdict__XXH_PRIME64_5                                                                  \
  0x27D4EB2F165667C5ULL /*!<                                                                  \
                           0b0010011111010100111010110010111100010110010101100110011111000101 \
                         */

#define cdict__XXH_get64bits(p) cdict__XXH_readLE64_align(p)

cdict__XXH_FORCE_INLINE cdict__xxh_u64 cdict__XXH_readLE64(const void *memPtr) {
  const cdict__xxh_u8 *bytePtr = (const cdict__xxh_u8 *)memPtr;
  return bytePtr[0] | ((cdict__xxh_u64)bytePtr[1] << 8) |
         ((cdict__xxh_u64)bytePtr[2] << 16) |
         ((cdict__xxh_u64)bytePtr[3] << 24) |
         ((cdict__xxh_u64)bytePtr[4] << 32) |
         ((cdict__xxh_u64)bytePtr[5] << 40) |
         ((cdict__xxh_u64)bytePtr[6] << 48) |
         ((cdict__xxh_u64)bytePtr[7] << 56);
}

#ifndef cdict__XXH_CPU_LITTLE_ENDIAN
#if defined(_WIN32) || defined(__LITTLE_ENDIAN__) ||                           \
    (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define cdict__XXH_CPU_LITTLE_ENDIAN 1
#elif defined(__BIG_ENDIAN__) ||                                               \
    (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define cdict__XXH_CPU_LITTLE_ENDIAN 0
#else

static int cdict__XXH_isLittleEndian(void) {
  const union {
    xxh_u32 u;
    xxh_u8 c[4];
  } one = {1};
  return one.c[0];
}
#define cdict__XXH_CPU_LITTLE_ENDIAN cdict__XXH_isLittleEndian()
#endif
#endif

cdict__XXH_FORCE_INLINE cdict__xxh_u64
cdict__XXH_readLE64_align(const void *ptr) {
  return cdict__XXH_readLE64(ptr);
}

#define cdict__XXH_rotl32(x, r) (((x) << (r)) | ((x) >> (32 - (r))))
#define cdict__XXH_rotl64(x, r) (((x) << (r)) | ((x) >> (64 - (r))))

typedef uint8_t cdict__xxh_u8;
typedef uint32_t cdict__XXH32_hash_t;
typedef uint32_t cdict__XXH32_hash_t;
typedef cdict__XXH32_hash_t cdict__xxh_u32;

static cdict__xxh_u32 cdict__XXH_swap32(cdict__xxh_u32 x) {
  return ((x << 24) & 0xff000000) | ((x << 8) & 0x00ff0000) |
         ((x >> 8) & 0x0000ff00) | ((x >> 24) & 0x000000ff);
}
static cdict__xxh_u32 cdict__XXH_read32(const void *memPtr) {
  cdict__xxh_u32 val;
  memcpy(&val, memPtr, sizeof(val));
  return val;
}

static cdict__xxh_u64 cdict__XXH64_round(cdict__xxh_u64 acc,
                                         cdict__xxh_u64 input) {
  acc += input * cdict__XXH_PRIME64_2;
  acc = cdict__XXH_rotl64(acc, 31);
  acc *= cdict__XXH_PRIME64_1;
  return acc;
}

static cdict__xxh_u64 cdict__XXH64_mergeRound(cdict__xxh_u64 acc,
                                              cdict__xxh_u64 val) {
  val = cdict__XXH64_round(0, val);
  acc ^= val;
  acc = acc * cdict__XXH_PRIME64_1 + cdict__XXH_PRIME64_4;
  return acc;
}

#define cdict__XXH_get32bits(p) cdict__XXH_readLE32_align(p)
cdict__XXH_FORCE_INLINE cdict__xxh_u32 cdict__XXH_readLE32(const void *ptr) {
  return cdict__XXH_CPU_LITTLE_ENDIAN
             ? cdict__XXH_read32(ptr)
             : cdict__XXH_swap32(cdict__XXH_read32(ptr));
}

cdict__XXH_FORCE_INLINE cdict__xxh_u32
cdict__XXH_readLE32_align(const void *ptr) {
  return cdict__XXH_readLE32(ptr);
}

static cdict__xxh_u64 cdict__XXH64_avalanche(cdict__xxh_u64 h64) {
  h64 ^= h64 >> 33;
  h64 *= cdict__XXH_PRIME64_2;
  h64 ^= h64 >> 29;
  h64 *= cdict__XXH_PRIME64_3;
  h64 ^= h64 >> 32;
  return h64;
}

static cdict__xxh_u64 cdict__XXH64_finalize(cdict__xxh_u64 h64,
                                            const cdict__xxh_u8 *ptr,
                                            size_t len) {
  len &= 31;
  while (len >= 8) {
    cdict__xxh_u64 const k1 = cdict__XXH64_round(0, cdict__XXH_get64bits(ptr));
    ptr += 8;
    h64 ^= k1;
    h64 = cdict__XXH_rotl64(h64, 27) * cdict__XXH_PRIME64_1 +
          cdict__XXH_PRIME64_4;
    len -= 8;
  }
  if (len >= 4) {
    h64 ^= (cdict__xxh_u64)(cdict__XXH_get32bits(ptr)) * cdict__XXH_PRIME64_1;
    ptr += 4;
    h64 = cdict__XXH_rotl64(h64, 23) * cdict__XXH_PRIME64_2 +
          cdict__XXH_PRIME64_3;
    len -= 4;
  }
  while (len > 0) {
    h64 ^= (*ptr++) * cdict__XXH_PRIME64_5;
    h64 = cdict__XXH_rotl64(h64, 11) * cdict__XXH_PRIME64_1;
    --len;
  }
  return cdict__XXH64_avalanche(h64);
}

cdict__XXH_FORCE_INLINE cdict__xxh_u64 cdict__XXH64_endian_align(
    const cdict__xxh_u8 *input, size_t len, cdict__xxh_u64 seed) {
  const cdict__xxh_u8 *bEnd = input ? input + len : NULL;
  cdict__xxh_u64 h64;

  if (len >= 32) {
    const cdict__xxh_u8 *const limit = bEnd - 32;
    cdict__xxh_u64 v1 = seed + cdict__XXH_PRIME64_1 + cdict__XXH_PRIME64_2;
    cdict__xxh_u64 v2 = seed + cdict__XXH_PRIME64_2;
    cdict__xxh_u64 v3 = seed + 0;
    cdict__xxh_u64 v4 = seed - cdict__XXH_PRIME64_1;

    do {
      v1 = cdict__XXH64_round(v1, cdict__XXH_get64bits(input));
      input += 8;
      v2 = cdict__XXH64_round(v2, cdict__XXH_get64bits(input));
      input += 8;
      v3 = cdict__XXH64_round(v3, cdict__XXH_get64bits(input));
      input += 8;
      v4 = cdict__XXH64_round(v4, cdict__XXH_get64bits(input));
      input += 8;
    } while (input <= limit);

    h64 = cdict__XXH_rotl64(v1, 1) + cdict__XXH_rotl64(v2, 7) +
          cdict__XXH_rotl64(v3, 12) + cdict__XXH_rotl64(v4, 18);
    h64 = cdict__XXH64_mergeRound(h64, v1);
    h64 = cdict__XXH64_mergeRound(h64, v2);
    h64 = cdict__XXH64_mergeRound(h64, v3);
    h64 = cdict__XXH64_mergeRound(h64, v4);

  } else {
    h64 = seed + cdict__XXH_PRIME64_5;
  }

  h64 += (cdict__xxh_u64)len;
  return cdict__XXH64_finalize(h64, input, len);
}

cdict__XXH_FORCE_INLINE cdict__xxh_u64 cdict__XXH64_endian_align_h(
    const cdict__xxh_u8 *input, size_t len, cdict__xxh_u64 seed) {
  const cdict__xxh_u8 *bEnd = input ? input + len : NULL;
  cdict__xxh_u64 h64;

  if (len >= 32) {
    const cdict__xxh_u8 *const limit = bEnd - 32;
    cdict__xxh_u64 v1 = seed + cdict__XXH_PRIME64_1 + cdict__XXH_PRIME64_2;
    cdict__xxh_u64 v2 = seed - cdict__XXH_PRIME64_2;
    cdict__xxh_u64 v3 = seed + cdict__XXH_PRIME64_3;
    cdict__xxh_u64 v4 = seed - cdict__XXH_PRIME64_1;

    do {
      v1 = cdict__XXH64_round(v1, cdict__XXH_get64bits(input));
      input += 8;
      v2 = cdict__XXH64_round(v2, cdict__XXH_get64bits(input));
      input += 8;
      v3 = cdict__XXH64_round(v3, cdict__XXH_get64bits(input));
      input += 8;
      v4 = cdict__XXH64_round(v4, cdict__XXH_get64bits(input));
      input += 8;
    } while (input <= limit);

    h64 = cdict__XXH_rotl64(v1, 1) + cdict__XXH_rotl64(v2, 7) +
          cdict__XXH_rotl64(v3, 12) + cdict__XXH_rotl64(v4, 18);
    h64 = cdict__XXH64_mergeRound(h64, v1);
    h64 = cdict__XXH64_mergeRound(h64, v2);
    h64 = cdict__XXH64_mergeRound(h64, v3);
    h64 = cdict__XXH64_mergeRound(h64, v4);

  } else {
    h64 = seed + cdict__XXH_PRIME64_1;
  }

  h64 += (cdict__xxh_u64)len;
  return cdict__XXH64_finalize(h64, input, len);
}

static cdict__XXH64_hash_t cdict__XXH64(const void *input, size_t len,
                                        cdict__XXH64_hash_t seed) {
  return cdict__XXH64_endian_align((const cdict__xxh_u8 *)input, len, seed);
}

static cdict__XXH64_hash_t cdict__XXH64_h(const void *input, size_t len,
                                          cdict__XXH64_hash_t seed) {
  return cdict__XXH64_endian_align_h((const cdict__xxh_u8 *)input, len, seed);
}

#ifndef CDICT__FORCE_INITIALIZE
#define CDICT__FORCE_INITIALIZE 1
#endif

#ifndef CDICT__INITIAL_CAP
#define CDICT__INITIAL_CAP 16
#endif

#ifndef CDICT__DEFAULT_SEED
#define CDICT__DEFAULT_SEED 2718182
#endif

#ifndef CDICT__MAX_LOAD_FACTOR
#define CDICT__MAX_LOAD_FACTOR 0.7
#endif

#ifndef CDICT__MIN_LOAD_FACTOR
#define CDICT__MIN_LOAD_FACTOR 0.2
#endif

#define cdict__ref(cdict) (&(cdict))

#define cdict__set_max_load_factor(cdict, value)                               \
  (((cdict)->cdict__max_load_factor_m) = (value))
#define cdict__set_min_load_factor(cdict, value)                               \
  (((cdict)->cdict__min_load_factor_m) = (value))

#define cdict__max_load_factor(cdict) ((cdict)->cdict__max_load_factor_m)
#define cdict__min_load_factor(cdict) ((cdict)->cdict__min_load_factor_m)

#define cdict__seed(cdict) ((cdict)->cdict__seed_m)
#define cdict__set_seed(cdict, value) (((cdict)->cdict__seed_m) = (value))

#define cdict__vector_buckets(cdict) (((cdict)->cdict__buckets_m))
#define cdict__vector_buckets_ref(cdict) (&((cdict)->cdict__buckets_m))

#define cdict__vector_temp_buckets_ref(cdict)                                  \
  (&((cdict)->cdict__temp_buckets_m))
#define cdict__vector_temp_buckets(cdict) ((cdict)->cdict__temp_buckets_m)

#define cdict__size(cdict) ((cdict)->cdict__bucket_size_m)
#define cdict__set_size(cdict, value)                                          \
  (((cdict)->cdict__bucket_size_m) = (value))

#define cdict__compare(cdict) (((cdict)->cdict__compare_m))
#define cdict__hash(cdict) (((cdict)->cdict__hash_m))

#define cdict__key(cdict) ((cdict)->cdict__key_m)
#define cdict__set_key_(cdict, key) ((cdict__key(cdict)) = (key))
#define cdict__key_ref(cdict) (&((cdict)->cdict__key_m))

#define cdict__cap(cdict) cdict_vector__cap(cdict__vector_buckets_ref(cdict))

#define cdict__Elem(cdict_key_type_, cdict_value_type_)                        \
  typedef struct {                                                             \
    int cdict__psl_m;                                                          \
    cdict_key_type_ key;                                                       \
    cdict_value_type_ val;                                                     \
  }

#define cdict__elem_key_ref(elem) (&((elem)->key))
#define cdict__elem_key(elem) ((elem)->key)

#define cdict__elem_val(elem) ((elem)->val)
#define cdict__elem_val_ref(elem) (&((elem)->val))

#define cdict__elem_psl(elem) ((elem)->cdict__psl_m)
#define cdict__set_elem_psl(elem, psl) (((elem)->cdict__psl_m) = (psl))

typedef uint8_t cdict__u8;
typedef uint64_t cdict__u64;

#define cdict__bytes_compare(self, other, size) (memcmp(self, other, size) == 0)

#define CDict(cdict_key_type_, cdict_value_type_)                              \
  cdict__Elem(cdict_key_type_, cdict_value_type_)                              \
      cdict_elem_##cdict_key_type_##cdict_value_type_;                         \
  cdict_Vector(cdict_elem_##cdict_key_type_##cdict_value_type_)                \
      buckets_##cdict_key_type_##cdict_value_type_;                            \
  typedef struct cdict_##cdict_key_type_##cdict_value_type_ {                  \
    buckets_##cdict_key_type_##cdict_value_type_ cdict__buckets_m;             \
    double cdict__max_load_factor_m;                                           \
    double cdict__min_load_factor_m;                                           \
    uint64_t cdict__seed_m;                                                    \
    cdict_key_type_ cdict__key_m;                                              \
    cdict_value_type_ cdict__value_m;                                          \
    size_t cdict__bucket_size_m;                                               \
    bool (*cdict__compare_m)(cdict_key_type_ * self, cdict_key_type_ *other);  \
    cdict__u64 (*cdict__hash_m)(cdict_key_type_ * self,                        \
                                cdict__u64 (*hash)(void *, size_t));           \
    buckets_##cdict_key_type_##cdict_value_type_ cdict__temp_buckets_m;        \
  }

#define cdict__set_hash(cdict, hasher) (((cdict)->cdict__hash_m) = (hasher))

#define cdict__set_comparator(cdict, comparator)                               \
  (((cdict)->cdict__compare_m) = (comparator))

#define cdict__init(cdict)                                                     \
  do {                                                                         \
    cdict__set_max_load_factor((cdict), (CDICT__MAX_LOAD_FACTOR));             \
    cdict__set_min_load_factor((cdict), (CDICT__MIN_LOAD_FACTOR));             \
    cdict__set_seed((cdict), (CDICT__DEFAULT_SEED));                           \
    cdict__set_size((cdict), (0));                                             \
    cdict__set_comparator((cdict), (NULL));                                    \
    cdict__set_hash((cdict), (NULL));                                          \
    cdict_vector__init_with_cap(cdict__vector_buckets_ref(cdict),              \
                                (CDICT__INITIAL_CAP));                         \
    if (CDICT__FORCE_INITIALIZE) {                                             \
      for (size_t cdict__i_m = 0;                                              \
           cdict__i_m < cdict_vector__cap(cdict__vector_buckets_ref(cdict));   \
           ((cdict__i_m)++)) {                                                 \
        cdict__set_elem_psl(                                                   \
            (cdict_vector__index(cdict__vector_buckets_ref(cdict),             \
                                 (cdict__i_m))),                               \
            0);                                                                \
      }                                                                        \
    }                                                                          \
  } while (0)

#define cdict__empty(vector_ref, index)                                        \
  (cdict__elem_psl(((cdict_vector__index((vector_ref), (index))))) == 0)

#define cdict__tombstone(vector_ref, index)                                    \
  (cdict__elem_psl(((cdict_vector__index((vector_ref), (index))))) == -1)

#define cdict__matches(cdict, vector_ref, ref, value, index)                   \
  (cdict__compare(cdict))                                                      \
      ? ((cdict__compare(cdict))(                                              \
            cdict__elem_key_ref(cdict_vector__index((vector_ref), (index))),   \
            (ref)))                                                            \
      : cdict__bytes_compare(                                                  \
            cdict__elem_key_ref(cdict_vector__index((vector_ref), (index))),   \
            (ref), sizeof(value))

static cdict__u64 cdict__hash1_callback(void *memptr, size_t size) {
  return cdict__XXH64(memptr, size, CDICT__DEFAULT_SEED);
}

static cdict__u64 cdict__hash2_callback(void *memptr, size_t size) {
  return ((cdict__XXH64_h(memptr, size, CDICT__DEFAULT_SEED)) | 1);
}

#define cdict__get_(cdict, ref, key, buffer)                                   \
  ({                                                                           \
    cdict__u64 cdict__h1_m = cdict__h1hash((cdict), (ref), (key));             \
    size_t cdict__iteration_m = 1;                                             \
    size_t cdict__index_m = 0;                                                 \
    bool cdict__found_m = false;                                               \
    for (;;) {                                                                 \
      /* break if we reach to the end */                                       \
      if ((cdict__iteration_m - 1) >= cdict__cap(cdict)) {                     \
        break;                                                                 \
      }                                                                        \
      cdict__u64 cdict__h2_m = cdict__h2hash((cdict), (ref), (key));           \
      cdict__index_m = cdict__double_hash_index((cdict__h1_m), (cdict__h2_m),  \
                                                (cdict__iteration_m - 1),      \
                                                cdict__cap(cdict));            \
      (cdict__iteration_m)++;                                                  \
      if (cdict__tombstone(cdict__vector_buckets_ref(cdict),                   \
                           cdict__index_m)) {                                  \
        continue;                                                              \
      }                                                                        \
      if (cdict__empty(cdict__vector_buckets_ref(cdict), (cdict__index_m))) {  \
        break;                                                                 \
      }                                                                        \
      bool cdict__matches_m =                                                  \
          cdict__matches((cdict), cdict__vector_buckets_ref(cdict), (ref),     \
                         (key), (cdict__index_m));                             \
      if (cdict__matches_m) {                                                  \
        cdict__found_m = true;                                                 \
        ((*(buffer)) = (cdict__elem_val((cdict_vector__index(                  \
             cdict__vector_buckets_ref(cdict), (cdict__index_m))))));          \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    (cdict__found_m);                                                          \
  })

#define cdict__get(cdict, key, buffer)                                         \
  ({                                                                           \
    (cdict__key(cdict) = (key));                                               \
    cdict__get_((cdict), cdict__key_ref(cdict), cdict__key(cdict), (buffer));  \
  })

#define cdict__contains_(cdict, ref, key)                                      \
  ({                                                                           \
    cdict__u64 cdict__h1_m = cdict__h1hash((cdict), (ref), (key));             \
    size_t cdict__iteration_m = 1;                                             \
    size_t cdict__index_m = 0;                                                 \
    bool cdict__found_m = false;                                               \
    for (;;) {                                                                 \
      /* break if we reach to the end */                                       \
      if ((cdict__iteration_m - 1) >= cdict__cap(cdict)) {                     \
        break;                                                                 \
      }                                                                        \
      cdict__u64 cdict__h2_m = cdict__h2hash((cdict), (ref), (key));           \
      cdict__index_m = cdict__double_hash_index(cdict__h1_m, cdict__h2_m,      \
                                                (cdict__iteration_m - 1),      \
                                                cdict__cap(cdict));            \
      (cdict__iteration_m)++;                                                  \
      if (cdict__tombstone(cdict__vector_buckets_ref(cdict),                   \
                           (cdict__index_m))) {                                \
        continue;                                                              \
      }                                                                        \
      if (cdict__empty(cdict__vector_buckets_ref(cdict), (cdict__index_m))) {  \
        break;                                                                 \
      }                                                                        \
      bool cdict__matches_m =                                                  \
          cdict__matches((cdict), cdict__vector_buckets_ref(cdict), (ref),     \
                         (key), (cdict__index_m));                             \
      if (cdict__matches_m) {                                                  \
        (cdict__found_m) = true;                                               \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    (cdict__found_m);                                                          \
  })

#define cdict__contains(cdict, key)                                            \
  ({                                                                           \
    (cdict__key(cdict) = (key));                                               \
    cdict__contains_((cdict), cdict__key_ref(cdict), cdict__key(cdict));       \
  })

#define cdict__h2hash(cdict, ref, key)                                         \
  ((cdict__hash(cdict)))                                                       \
      ? (((cdict__hash(cdict)))((ref), cdict__hash2_callback))                 \
      : ((cdict__XXH64_h((ref), sizeof(key), cdict__seed(cdict))) | 1);

#define cdict__h1hash(cdict, ref, key)                                         \
  ((cdict__hash(cdict)))                                                       \
      ? (((cdict__hash(cdict)))((ref), cdict__hash1_callback))                 \
      : (cdict__XXH64((ref), sizeof(key), (cdict__seed(cdict))))

// NOTE: & works instead of % because cap is power of 2 i.e mod(cap , 2) = 0
#define cdict__double_hash_index(ha1, ha2, i, cap) (((ha1) + ((i) * (ha2))) & (cap-1))

#define cdict__add(cdict, key, val)                                            \
  do {                                                                         \
    if ((((double)(cdict__size(cdict)) / (cdict__cap(cdict))) >=               \
         (cdict__max_load_factor(cdict)))) {                                   \
      cdict__resize((cdict), (cdict__cap(cdict) * 2));                         \
    }                                                                          \
    (cdict__key(cdict)) = (key);                                               \
    cdict__add_((cdict), cdict__vector_buckets_ref(cdict),                     \
                cdict__key_ref(cdict), cdict__key(cdict), (val));              \
  } while (0)

#define cdict__add_(cdict, vector_ref, key_ref, key, value)                    \
  do {                                                                         \
    cdict__u64 cdict__h1 = cdict__h1hash((cdict), (key_ref), (key));           \
    size_t cdict__iteration_m = 1;                                             \
    bool cdict__found_m = false;                                               \
    size_t cdict__index_m;                                                     \
    for (;;) {                                                                 \
      cdict__u64 cdict__h2 = cdict__h2hash((cdict), (key_ref), (key));         \
      cdict__index_m = cdict__double_hash_index(                               \
          (cdict__h1), (cdict__h2), (cdict__iteration_m - 1),                  \
          cdict_vector__cap(vector_ref));                                      \
      (cdict__iteration_m)++;                                                  \
      if (cdict__empty((vector_ref), (cdict__index_m)) ||                      \
          cdict__tombstone((vector_ref), (cdict__index_m))) {                  \
        break;                                                                 \
      }                                                                        \
      bool cdict__matches_m = cdict__matches((cdict), (vector_ref), (key_ref), \
                                             (key), (cdict__index_m));         \
      if (cdict__matches_m) {                                                  \
        cdict__found_m = true;                                                 \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    cdict__set_at_index((vector_ref), (cdict__index_m), (key), (value),        \
                        (cdict__iteration_m));                                 \
    if ((!(cdict__found_m))) {                                                 \
      cdict__set_size((cdict), ((cdict__size(cdict)) + 1));                    \
    }                                                                          \
  } while (0)

#define cdict__set_key_at_index(vector_ref, index, key)                        \
  (((cdict__elem_key(cdict_vector__index((vector_ref), (index)))) = (key)))
#define cdict__set_value_at_index(vector_ref, index, value)                    \
  (((cdict__elem_val(cdict_vector__index((vector_ref), (index)))) = (value)))
#define cdict__set_psl_at_index(vector_ref, index, psl)                        \
  (((cdict__elem_psl(cdict_vector__index((vector_ref), (index)))) = (psl)))

#define cdict__set_at_index(vector_ref, index, key, value, psl)                \
  do {                                                                         \
    cdict__set_key_at_index((vector_ref), (index), (key));                     \
    cdict__set_value_at_index((vector_ref), (index), (value));                 \
    cdict__set_psl_at_index((vector_ref), (index), (psl));                     \
  } while (0)

#define cdict__resize(cdict, cap)                                              \
  do {                                                                         \
    cdict_vector__init_with_cap(cdict__vector_temp_buckets_ref(cdict), (cap)); \
    for (size_t cdict__i_m = 0;                                                \
         cdict__i_m <                                                          \
         cdict_vector__cap(cdict__vector_temp_buckets_ref(cdict));             \
         (cdict__i_m)++) {                                                     \
      (cdict__set_elem_psl(                                                    \
          (cdict_vector__index(cdict__vector_temp_buckets_ref(cdict),          \
                               (cdict__i_m))),                                 \
          0));                                                                 \
    }                                                                          \
    /* reset the size of cdict */                                              \
    cdict__set_size((cdict), 0);                                               \
    size_t cdict__current_index = 0;                                           \
    for (;;) {                                                                 \
      if (cdict__current_index >= (cdict__cap(cdict))) {                       \
        break;                                                                 \
      }                                                                        \
      int cdict__psl_m = cdict__elem_psl(cdict_vector__index(                  \
          (cdict__vector_buckets_ref(cdict)), (cdict__current_index)));        \
      if (((cdict__psl_m) == 0) || ((cdict__psl_m) == -1)) {                   \
        (cdict__current_index)++;                                              \
        continue;                                                              \
      }                                                                        \
      cdict__add_(                                                             \
          (cdict), (cdict__vector_temp_buckets_ref(cdict)),                    \
          cdict__elem_key_ref(cdict_vector__index(                             \
              (cdict__vector_buckets_ref(cdict)), (cdict__current_index))),    \
          cdict__elem_key(cdict_vector__index(                                 \
              (cdict__vector_buckets_ref(cdict)), (cdict__current_index))),    \
          cdict__elem_val(cdict_vector__index(                                 \
              cdict__vector_buckets_ref(cdict), (cdict__current_index))));     \
      (cdict__current_index)++;                                                \
    }                                                                          \
    cdict__free(cdict);                                                        \
    ((cdict__vector_buckets(cdict)) = ((cdict__vector_temp_buckets(cdict))));  \
  } while (0)

#define cdict__remove_(cdict, ref, key, vector_ref)                            \
  ({                                                                           \
    cdict__u64 cdict__h1_m = cdict__h1hash((cdict), (ref), (key));             \
    bool cdict__found_m = false;                                               \
    size_t cdict__iteration_m = 1;                                             \
    size_t cdict__index_m;                                                     \
    size_t cap = cdict_vector__cap(vector_ref);                                \
    for (;;) {                                                                 \
      if (((cdict__iteration_m)-1) >= cdict_vector__cap(vector_ref))           \
        break;                                                                 \
      cdict__u64 cdict__h2_m = cdict__h2hash((cdict), (ref), (key));           \
      cdict__index_m = cdict__double_hash_index(                               \
          (cdict__h1_m), (cdict__h2_m), ((cdict__iteration_m)-1),              \
          cdict_vector__cap(vector_ref));                                      \
      (cdict__iteration_m)++;                                                  \
      if (cdict__tombstone((vector_ref), (cdict__index_m))) {                  \
        continue;                                                              \
      }                                                                        \
      if (cdict__empty((vector_ref), (cdict__index_m))) {                      \
        break;                                                                 \
      }                                                                        \
      bool cdict__matches_m = cdict__matches((cdict), (vector_ref), (ref),     \
                                             (key), (cdict__index_m));         \
      if (cdict__matches_m) {                                                  \
        cdict__found_m = true;                                                 \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    if (cdict__found_m) {                                                      \
      (cdict__set_elem_psl(                                                    \
          (cdict_vector__index((vector_ref), (cdict__index_m))), -1));         \
      cdict__set_size((cdict), (cdict__size(cdict)) - 1);                      \
    }                                                                          \
    ((cdict__found_m));                                                        \
  })

#define cdict__remove(cdict, key)                                              \
  ({                                                                           \
    (cdict__key(cdict) = (key));                                               \
    cdict__remove_(cdict, cdict__key_ref(cdict), cdict__key(cdict),            \
                   cdict__vector_buckets_ref(cdict));                          \
  })

#define cdict__pop(cdict, key, buffer)                                         \
  ({                                                                           \
    cdict__set_key_((cdict), (key));                                           \
    cdict__get((cdict), cdict__key(cdict), (buffer));                          \
    cdict__remove((cdict), cdict__key(cdict));                                 \
  })

#define cdict__clear(cdict)                                                    \
  do {                                                                         \
    cdict__free(cdict);                                                        \
    cdict_vector__init_with_cap(cdict__vector_buckets_ref(cdict),              \
                                (CDICT__INITIAL_CAP));                         \
    cdict__set_size((cdict), 0);                                               \
                                                                               \
    if (CDICT__FORCE_INITIALIZE) {                                             \
      for (size_t cdict__i_m = 0;                                              \
           cdict__i_m < (cdict_vector__cap(cdict__vector_buckets_ref(cdict))); \
           (cdict__i_m)++) {                                                   \
        (cdict__set_elem_psl(                                                  \
            (cdict_vector__index(cdict__vector_buckets_ref(cdict),             \
                                 (cdict__i_m))),                               \
            0));                                                               \
      }                                                                        \
    }                                                                          \
  } while (0)

#define cdict__free(cdict) cdict_vector__free(cdict__vector_buckets_ref(cdict))

/* Cdict_iterator */

#define CDict_iterator(cdict_type_)                                            \
  typedef struct cdict_iterator_##cdict_type_ {                                \
    cdict_type_ *cdict__m;                                                     \
    size_t cdict__current_count_m;                                             \
    size_t cdict__current_index_m;                                             \
  }

#define cdict_iterator__m(iterator) ((iterator)->cdict__m)
#define cdict_iterator__set_m(iterator, cdict)                                 \
  ((cdict_iterator__m(iterator)) = (cdict))

#define cdict_iterator__current_count(iterator)                                \
  ((iterator)->cdict__current_count_m)
#define cdict_iterator__set_count(iterator, count)                             \
  (cdict_iterator__current_count(iterator) = (count))

#define cdict_iterator__current_index(iterator)                                \
  ((iterator)->cdict__current_index_m)
#define cdict_iterator__set_index(iterator, index)                             \
  ((cdict_iterator__current_index(iterator)) = (index))

#define cdict_iterator__init(iterator, cdict)                                  \
  do {                                                                         \
    cdict_iterator__set_m((iterator), (cdict));                                \
    cdict_iterator__set_index((iterator), 0);                                  \
    cdict_iterator__set_count((iterator), 0);                                  \
  } while (0)

#define cdict_iterator__done(iterator)                                         \
  (((iterator)->cdict__current_count_m) >=                                     \
   ((cdict__size(((iterator)->cdict__m)))))

#define cdict_iterator__next(iterator)                                         \
  ({                                                                           \
    for (;;) {                                                                 \
      int cdict__psl_m = cdict__elem_psl(cdict_vector__index(                  \
          (cdict__vector_buckets_ref(cdict_iterator__m(iterator))),            \
          cdict_iterator__current_index(iterator)));                           \
      if (cdict__psl_m == 0 || cdict__psl_m == -1) {                           \
        ((cdict_iterator__current_index(iterator))++);                         \
        continue;                                                              \
      }                                                                        \
      ((cdict_iterator__current_count(iterator))++);                           \
      ((cdict_iterator__current_index(iterator))++);                           \
      break;                                                                   \
    }                                                                          \
    (cdict__elem_key(cdict_vector__index(                                      \
        (cdict__vector_buckets_ref((cdict_iterator__m(iterator)))),            \
        (cdict_iterator__current_index(iterator) - 1))));                      \
  })

#define cdict_iterator__next_val(iterator)                                     \
  ({                                                                           \
    for (;;) {                                                                 \
      int cdict__psl_m = cdict__elem_psl(cdict_vector__index(                  \
          (cdict__vector_buckets_ref(cdict_iterator__m(iterator))),            \
          cdict_iterator__current_index(iterator)));                           \
      if ((cdict__psl_m == 0) || (cdict__psl_m == -1)) {                       \
        ((cdict_iterator__current_index(iterator))++);                         \
        continue;                                                              \
      }                                                                        \
      (cdict_iterator__current_count(iterator))++;                             \
      (cdict_iterator__current_index(iterator))++;                             \
      break;                                                                   \
    }                                                                          \
    (cdict__elem_val(cdict_vector__index(                                      \
        (cdict__vector_buckets_ref((cdict_iterator__m(iterator)))),            \
        (cdict_iterator__current_index(iterator) - 1))));                      \
  })

#define cdict_iterator__next_keyval(iterator, value)                           \
  ({                                                                           \
    for (;;) {                                                                 \
      int cdict__psl_m = cdict__elem_psl(cdict_vector__index(                  \
          (cdict__vector_buckets_ref(cdict_iterator__m(iterator))),            \
          cdict_iterator__current_index(iterator)));                           \
      if ((cdict__psl_m == 0) || (cdict__psl_m == -1)) {                       \
        ((cdict_iterator__current_index(iterator))++);                         \
        continue;                                                              \
      }                                                                        \
      (*(value)) = (cdict__elem_val(cdict_vector__index(                       \
          (cdict__vector_buckets_ref((cdict_iterator__m(iterator)))),          \
          (cdict_iterator__current_index(iterator)))));                        \
      (cdict_iterator__current_count(iterator))++;                             \
      (cdict_iterator__current_index(iterator))++;                             \
      break;                                                                   \
    }                                                                          \
    (cdict__elem_key(cdict_vector__index(                                      \
        (cdict__vector_buckets_ref((cdict_iterator__m(iterator)))),            \
        (cdict_iterator__current_index(iterator) - 1))));                      \
  })

#define cdict__fromkeys(cdict, buffer, size, defval)                           \
  do {                                                                         \
    for (size_t cdict__i_m = 0; cdict__i_m < (size); (cdict__i_m)++) {         \
      cdict__add(cdict, (buffer[(cdict__i_m)]), (defval));                     \
    }                                                                          \
  } while (0)

/* Vector required by cdict */

#define cdict_Vector(Type_)                                                    \
  typedef struct {                                                             \
    Type_ *cdict_vector__elem_m;                                               \
    size_t cdict_vector__size_m;                                               \
    size_t cdict_vector__cap_m;                                                \
    bool cdict_vector__initialized_m;                                          \
  }

#define cdict_vector__init_with_cap(tv, ncap)                                  \
  do {                                                                         \
    ((tv)->cdict_vector__size_m) = 0;                                          \
    ((tv)->cdict_vector__elem_m) = NULL;                                       \
    ((tv)->cdict_vector__cap_m) = 0;                                           \
    cdict_vector__grow((tv), (ncap));                                          \
    ((tv)->cdict_vector__initialized_m) = true;                                \
  } while (0)

#define cdict_vector__grow(tv, ncap)                                           \
  do {                                                                         \
    size_t cdict__cap_m = ((tv)->cdict_vector__cap_m);                         \
    void *cdict__mem_m =                                                       \
        malloc(sizeof(*((tv)->cdict_vector__elem_m)) * (ncap));                \
    memcpy((cdict__mem_m), ((tv)->cdict_vector__elem_m),                       \
           sizeof(*((tv)->cdict_vector__elem_m)) * (cdict__cap_m));            \
    free((tv)->cdict_vector__elem_m);                                          \
    ((tv)->cdict_vector__elem_m) = (cdict__mem_m);                             \
    ((tv)->cdict_vector__cap_m) = (ncap);                                      \
  } while (0)

#define cdict_vector__add(tv, value)                                           \
  do {                                                                         \
    size_t cdict__size_m = ((tv)->cdict_vector__size_m);                       \
    size_t cdict__cap_m = ((tv)->cdict_vector__cap_m);                         \
    if (((cdict__size_m) >= (cdict__cap_m))) {                                 \
      size_t cdict__new_cap_m =                                                \
          ((cdict__cap_m) == 0) ? 1 : ((cdict__cap_m)*2);                      \
      cdict_vector__grow((tv), (cdict__new_cap_m));                            \
    }                                                                          \
    (((tv)->cdict_vector__elem_m)[cdict__size_m]) = (value);                   \
    (((tv)->cdict_vector__size_m)++);                                          \
  } while (0)

#define cdict_vector__index(vec, i) &(((vec)->cdict_vector__elem_m)[(i)])

#define cdict_vector__cap(vec) ((vec)->cdict_vector__cap_m)

#define cdict_vector__elem(vec) ((vec)->cdict_vector__elem_m)

#define cdict_vector__set_elem(vec, value) ((cdict_vector__elem(vec)) = (value))

#define cdict_vector__free(vec)                                                \
  do {                                                                         \
    free(cdict_vector__elem(vec));                                             \
    cdict_vector__set_elem((vec), NULL);                                       \
  } while (0)

#endif /* CDICT_H */

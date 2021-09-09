#ifndef CSET_H
#define CSET_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* xxhash algorithm */

typedef uint64_t XXH64_hash_t;
typedef uint8_t xxh_u8;

typedef XXH64_hash_t xxh_u64;

#define XXH_FORCE_INLINE                                                       \
  static __inline__ __attribute__((always_inline, unused))

#define XXH_PRIME64_1                                                                         \
  0x9E3779B185EBCA87ULL /*!<                                                                  \
                           0b1001111000110111011110011011000110000101111010111100101010000111 \
                         */
#define XXH_PRIME64_2                                                                         \
  0xC2B2AE3D27D4EB4FULL /*!<                                                                  \
                           0b1100001010110010101011100011110100100111110101001110101101001111 \
                         */
#define XXH_PRIME64_3                                                                         \
  0x165667B19E3779F9ULL /*!<                                                                  \
                           0b0001011001010110011001111011000110011110001101110111100111111001 \
                         */
#define XXH_PRIME64_4                                                                         \
  0x85EBCA77C2B2AE63ULL /*!<                                                                  \
                           0b1000010111101011110010100111011111000010101100101010111001100011 \
                         */
#define XXH_PRIME64_5                                                                         \
  0x27D4EB2F165667C5ULL /*!<                                                                  \
                           0b0010011111010100111010110010111100010110010101100110011111000101 \
                         */

#define XXH_get64bits(p) XXH_readLE64_align(p)

XXH_FORCE_INLINE xxh_u64 XXH_readLE64(const void *memPtr) {
  const xxh_u8 *bytePtr = (const xxh_u8 *)memPtr;
  return bytePtr[0] | ((xxh_u64)bytePtr[1] << 8) | ((xxh_u64)bytePtr[2] << 16) |
         ((xxh_u64)bytePtr[3] << 24) | ((xxh_u64)bytePtr[4] << 32) |
         ((xxh_u64)bytePtr[5] << 40) | ((xxh_u64)bytePtr[6] << 48) |
         ((xxh_u64)bytePtr[7] << 56);
}

#ifndef XXH_CPU_LITTLE_ENDIAN
#if defined(_WIN32) || defined(__LITTLE_ENDIAN__) ||                           \
    (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define XXH_CPU_LITTLE_ENDIAN 1
#elif defined(__BIG_ENDIAN__) ||                                               \
    (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define XXH_CPU_LITTLE_ENDIAN 0
#else
static int XXH_isLittleEndian(void) {
  const union {
    xxh_u32 u;
    xxh_u8 c[4];
  } one = {1};
  return one.c[0];
}
#define XXH_CPU_LITTLE_ENDIAN XXH_isLittleEndian()
#endif
#endif

XXH_FORCE_INLINE xxh_u64 XXH_readLE64_align(const void *ptr) {
  return XXH_readLE64(ptr);
}

#define XXH_rotl32(x, r) (((x) << (r)) | ((x) >> (32 - (r))))
#define XXH_rotl64(x, r) (((x) << (r)) | ((x) >> (64 - (r))))

typedef uint8_t xxh_u8;
typedef uint32_t XXH32_hash_t;
typedef uint32_t XXH32_hash_t;
typedef XXH32_hash_t xxh_u32;

static xxh_u32 XXH_swap32(xxh_u32 x) {
  return ((x << 24) & 0xff000000) | ((x << 8) & 0x00ff0000) |
         ((x >> 8) & 0x0000ff00) | ((x >> 24) & 0x000000ff);
}
static xxh_u32 XXH_read32(const void *memPtr) {
  xxh_u32 val;
  memcpy(&val, memPtr, sizeof(val));
  return val;
}

static xxh_u64 XXH64_round(xxh_u64 acc, xxh_u64 input) {
  acc += input * XXH_PRIME64_2;
  acc = XXH_rotl64(acc, 31);
  acc *= XXH_PRIME64_1;
  return acc;
}

static xxh_u64 XXH64_mergeRound(xxh_u64 acc, xxh_u64 val) {
  val = XXH64_round(0, val);
  acc ^= val;
  acc = acc * XXH_PRIME64_1 + XXH_PRIME64_4;
  return acc;
}

#define XXH_get32bits(p) XXH_readLE32_align(p)
XXH_FORCE_INLINE xxh_u32 XXH_readLE32(const void *ptr) {
  return XXH_CPU_LITTLE_ENDIAN ? XXH_read32(ptr) : XXH_swap32(XXH_read32(ptr));
}

XXH_FORCE_INLINE xxh_u32 XXH_readLE32_align(const void *ptr) {
  return XXH_readLE32(ptr);
}

static xxh_u64 XXH64_avalanche(xxh_u64 h64) {
  h64 ^= h64 >> 33;
  h64 *= XXH_PRIME64_2;
  h64 ^= h64 >> 29;
  h64 *= XXH_PRIME64_3;
  h64 ^= h64 >> 32;
  return h64;
}

static xxh_u64 XXH64_finalize(xxh_u64 h64, const xxh_u8 *ptr, size_t len) {
  len &= 31;
  while (len >= 8) {
    xxh_u64 const k1 = XXH64_round(0, XXH_get64bits(ptr));
    ptr += 8;
    h64 ^= k1;
    h64 = XXH_rotl64(h64, 27) * XXH_PRIME64_1 + XXH_PRIME64_4;
    len -= 8;
  }
  if (len >= 4) {
    h64 ^= (xxh_u64)(XXH_get32bits(ptr)) * XXH_PRIME64_1;
    ptr += 4;
    h64 = XXH_rotl64(h64, 23) * XXH_PRIME64_2 + XXH_PRIME64_3;
    len -= 4;
  }
  while (len > 0) {
    h64 ^= (*ptr++) * XXH_PRIME64_5;
    h64 = XXH_rotl64(h64, 11) * XXH_PRIME64_1;
    --len;
  }
  return XXH64_avalanche(h64);
}

XXH_FORCE_INLINE xxh_u64 XXH64_endian_align(const xxh_u8 *input, size_t len,
                                            xxh_u64 seed) {
  const xxh_u8 *bEnd = input ? input + len : NULL;
  xxh_u64 h64;

  if (len >= 32) {
    const xxh_u8 *const limit = bEnd - 32;
    xxh_u64 v1 = seed + XXH_PRIME64_1 + XXH_PRIME64_2;
    xxh_u64 v2 = seed + XXH_PRIME64_2;
    xxh_u64 v3 = seed + 0;
    xxh_u64 v4 = seed - XXH_PRIME64_1;

    do {
      v1 = XXH64_round(v1, XXH_get64bits(input));
      input += 8;
      v2 = XXH64_round(v2, XXH_get64bits(input));
      input += 8;
      v3 = XXH64_round(v3, XXH_get64bits(input));
      input += 8;
      v4 = XXH64_round(v4, XXH_get64bits(input));
      input += 8;
    } while (input <= limit);

    h64 = XXH_rotl64(v1, 1) + XXH_rotl64(v2, 7) + XXH_rotl64(v3, 12) +
          XXH_rotl64(v4, 18);
    h64 = XXH64_mergeRound(h64, v1);
    h64 = XXH64_mergeRound(h64, v2);
    h64 = XXH64_mergeRound(h64, v3);
    h64 = XXH64_mergeRound(h64, v4);

  } else {
    h64 = seed + XXH_PRIME64_5;
  }

  h64 += (xxh_u64)len;
  return XXH64_finalize(h64, input, len);
}

XXH_FORCE_INLINE xxh_u64 XXH64_endian_align_h(const xxh_u8 *input, size_t len,
                                              xxh_u64 seed) {
  const xxh_u8 *bEnd = input ? input + len : NULL;
  xxh_u64 h64;

  if (len >= 32) {
    const xxh_u8 *const limit = bEnd - 32;
    xxh_u64 v1 = seed + XXH_PRIME64_1 + XXH_PRIME64_2;
    xxh_u64 v2 = seed - XXH_PRIME64_2;
    xxh_u64 v3 = seed + XXH_PRIME64_3;
    xxh_u64 v4 = seed - XXH_PRIME64_1;

    do {
      v1 = XXH64_round(v1, XXH_get64bits(input));
      input += 8;
      v2 = XXH64_round(v2, XXH_get64bits(input));
      input += 8;
      v3 = XXH64_round(v3, XXH_get64bits(input));
      input += 8;
      v4 = XXH64_round(v4, XXH_get64bits(input));
      input += 8;
    } while (input <= limit);

    h64 = XXH_rotl64(v1, 1) + XXH_rotl64(v2, 7) + XXH_rotl64(v3, 12) +
          XXH_rotl64(v4, 18);
    h64 = XXH64_mergeRound(h64, v1);
    h64 = XXH64_mergeRound(h64, v2);
    h64 = XXH64_mergeRound(h64, v3);
    h64 = XXH64_mergeRound(h64, v4);

  } else {
    h64 = seed + XXH_PRIME64_1;
  }

  h64 += (xxh_u64)len;
  return XXH64_finalize(h64, input, len);
}

static XXH64_hash_t XXH64(const void *input, size_t len, XXH64_hash_t seed) {
  return XXH64_endian_align((const xxh_u8 *)input, len, seed);
}

static XXH64_hash_t XXH64_h(const void *input, size_t len, XXH64_hash_t seed) {
  return XXH64_endian_align_h((const xxh_u8 *)input, len, seed);
}

#ifndef CSET__FORCE_INITIALIZE
#define CSET__FORCE_INITIALIZE 1
#endif

#ifndef cset__INITIAL_CAP
#define cset__INITIAL_CAP 2
#endif

#ifndef cset__DEFAULT_SEED
#define cset__DEFAULT_SEED 2718182
#endif

#ifndef cset__MAX_LOAD_FACTOR
#define cset__MAX_LOAD_FACTOR 0.7
#endif

#ifndef cset__MIN_LOAD_FACTOR
#define cset__MIN_LOAD_FACTOR 0.2
#endif

#define cset__ref(cset) (&(cset))

#define cset__set_max_load_factor(cset, value)                                 \
  (((cset)->max_load_factor) = (value))
#define cset__set_min_load_factor(cset, value)                                 \
  (((cset)->min_load_factor) = (value))

#define cset__max_load_factor(cset) ((cset)->max_load_factor)
#define cset__min_load_factor(cset) ((cset)->min_load_factor)

#define cset__seed(cset) ((cset)->seed)
#define cset__set_seed(cset, value) (((cset)->seed) = (value))

#define cset__vector_buckets(cset) (((cset)->buckets))
#define cset__vector_buckets_ref(cset) (&((cset)->buckets))

#define cset__vector_temp_buckets_ref(cset) (&((cset)->temp_buckets))

#define cset__size(cset) ((cset)->bucket_size)
#define cset__set_size(cset, value) (((cset)->bucket_size) = value)

#define cset__cap(cset) cset_vector__cap(cset__vector_buckets_ref(cset))

#define cset__Value(cset_type_)                                                \
  typedef struct {                                                             \
    int pi;                                                                    \
    cset_type_ elem;                                                           \
  }

#define cset__value_elem_ref(val) (&((val)->elem))
#define cset__value_elem(val) ((val)->elem)

#define cset__value_pi(val) ((val)->pi)

typedef uint8_t cset__u8;
typedef uint64_t cset__u64;

#define cset__bytes_compare(self, other, size) (memcmp(self, other, size) == 0)

#define Cset(cset_type_)                                                       \
  cset__Value(cset_type_) cset_value_##cset_type_;                             \
  cset_Vector(cset_value_##cset_type_) buckets_##cset_type_;                   \
  typedef struct cset_##cset_type_ {                                           \
    buckets_##cset_type_ buckets;                                              \
    double max_load_factor;                                                    \
    double min_load_factor;                                                    \
    uint64_t seed;                                                             \
    cset_type_ v;                                                              \
    size_t bucket_size;                                                        \
    bool (*compare)(cset_type_ * self, cset_type_ *other);                     \
    cset__u64 (*customhasher)(cset_type_ * self,                               \
                              cset__u64 (*hash)(void *, size_t));              \
    buckets_##cset_type_ temp_buckets;                                         \
  }

#define cset__set_hash(cset, hasher) (((cset)->customhasher) = hasher)

#define cset__set_comparator(cset, comparator) (((cset)->compare) = comparator)

#define cset__init(cset)                                                       \
  do {                                                                         \
    cset__set_max_load_factor(cset, cset__MAX_LOAD_FACTOR);                    \
    cset__set_min_load_factor(cset, cset__MIN_LOAD_FACTOR);                    \
    cset__set_seed(cset, cset__DEFAULT_SEED);                                  \
    cset__set_size(cset, 0);                                                   \
    cset__set_comparator(cset, NULL);                                          \
    cset__set_hash(cset, NULL);                                                \
    cset_vector__init_with_cap(cset__vector_buckets_ref(cset),                 \
                               cset__INITIAL_CAP);                             \
    if (CSET__FORCE_INITIALIZE) {                                              \
      for (size_t i = 0; i < cset_vector__cap(cset__vector_buckets_ref(cset)); \
           i++) {                                                              \
        ((cset_vector__index(cset__vector_buckets_ref(cset), i))->pi) = 0;     \
      }                                                                        \
    }                                                                          \
  } while (0)

#define cset__empty(vector_ref, index)                                         \
  (((cset_vector__index(vector_ref, index))->pi) == 0)

#define cset__tombstone(vector_ref, index)                                     \
  (((cset_vector__index(vector_ref, index))->pi) == -1)

#define cset__index(cset, index)                                               \
  &(((cset_vector__index(cset__vector_buckets_ref(cset), index))->elem))

#define cset__matches(cset, vector_ref, ref, value, index)                     \
  ((cset)->compare)                                                            \
      ? (((cset)->compare)(                                                    \
            cset__value_elem_ref(cset_vector__index((vector_ref), (index))),   \
            (ref)))                                                            \
      : cset__bytes_compare(                                                   \
            cset__value_elem_ref(cset_vector__index((vector_ref), (index))),   \
            (ref), sizeof(value))

static cset__u64 cset__hash1_callback(void *memptr, size_t size) {
  return XXH64(memptr, size, cset__DEFAULT_SEED);
}

static cset__u64 cset__hash2_callback(void *memptr, size_t size) {
  return ((XXH64_h(memptr, size, cset__DEFAULT_SEED)) | 1);
}

#define cset__contains_(cset, ref, value, flag)                                \
  do {                                                                         \
    ((cset)->v) = value;                                                       \
    cset__u64 h1 = cset__h1hash(cset, ref, value);                             \
    size_t cap = cset__cap(cset);                                              \
    size_t iteration = 1;                                                      \
    size_t index = 0;                                                          \
    bool found = false;                                                        \
    for (;;) {                                                                 \
      /* break if we reach to the end */                                       \
      if ((iteration - 1) >= cset__cap(cset)) {                                \
        break;                                                                 \
      }                                                                        \
      cset__u64 h2 = cset__h2hash(cset, ref, value);                           \
      index = cset__double_hash_index(h1, h2, (iteration - 1), cap);           \
      iteration++;                                                             \
      if (cset__tombstone(cset__vector_buckets_ref(cset), index)) {            \
        continue;                                                              \
      }                                                                        \
      if (cset__empty(cset__vector_buckets_ref(cset), index)) {                \
        break;                                                                 \
      }                                                                        \
      bool matches = cset__matches(cset, cset__vector_buckets_ref(cset), ref,  \
                                   value, index);                              \
      if (matches) {                                                           \
        found = true;                                                          \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    (*(flag)) = found;                                                         \
  } while (0)

#define cset__contains(cset, value, flag)                                      \
  do {                                                                         \
    ((cset)->v) = value;                                                       \
    cset__contains_(cset, (&((cset)->v)), ((cset)->v), flag);                  \
  } while (0)

#define cset__h2hash(cset, ref, value)                                         \
  (((cset)->customhasher))                                                     \
      ? ((((cset)->customhasher))((ref), cset__hash2_callback))                \
      : ((XXH64_h((ref), sizeof(value), cset__seed(cset))) | 1)

#define cset__h1hash(cset, ref, value)                                         \
  (((cset)->customhasher))                                                     \
      ? ((((cset)->customhasher))(ref, cset__hash1_callback))                  \
      : (XXH64((ref), sizeof(value), (cset__seed(cset))))

#define cset__double_hash_index(ha1, ha2, i, cap)                              \
  (((ha1) + ((i) * (ha2))) % cap)

#define cset__add(cset, value)                                                 \
  do {                                                                         \
    double current_load_factor =                                               \
        ((double)(cset__size(cset)) / (cset__cap(cset)));                      \
    if (current_load_factor >= cset__max_load_factor(cset)) {                  \
      cset__resize(cset, (cset__cap(cset) * 2));                               \
    }                                                                          \
    ((cset)->v) = value;                                                       \
    cset__add_((cset), &((cset)->v), cset__vector_buckets_ref(cset),           \
               ((cset)->v));                                                   \
  } while (0)

#define cset__add_(cset, ref, vector_ref, value)                               \
  do {                                                                         \
    cset__u64 h1 = cset__h1hash(cset, ref, *ref);                              \
    bool found = false;                                                        \
    size_t iteration = 1;                                                      \
    size_t index;                                                              \
    size_t cap = cset_vector__cap(vector_ref);                                 \
    for (;;) {                                                                 \
      cset__u64 h2 = cset__h2hash(cset, ref, *ref);                            \
      index = cset__double_hash_index(h1, h2, (iteration - 1), cap);           \
      iteration++;                                                             \
      if (cset__empty(vector_ref, index) ||                                    \
          cset__tombstone(vector_ref, index)) {                                \
        break;                                                                 \
      }                                                                        \
      bool matches = cset__matches(cset, vector_ref, ref, value, index);       \
      if (matches) {                                                           \
        found = true;                                                          \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    if (!found) {                                                              \
      ((cset_vector__index(vector_ref, index))->elem) = value;                 \
      ((cset_vector__index(vector_ref, index))->pi) = iteration;               \
      cset__set_size(cset, (cset__size(cset)) + 1);                            \
    }                                                                          \
  } while (0)

#define cset__resize(cset, cap)                                                \
  do {                                                                         \
    size_t current_cap = cset__cap(cset);                                      \
    cset_vector__init_with_cap(&((cset)->temp_buckets), cap);                  \
    for (size_t i = 0;                                                         \
         i < cset_vector__cap(cset__vector_temp_buckets_ref(cset)); i++) {     \
      ((cset_vector__index(cset__vector_temp_buckets_ref(cset), i))->pi) = 0;  \
    }                                                                          \
    /* reset the size of cset */                                               \
    cset__set_size(cset, 0);                                                   \
    size_t current_index = 0;                                                  \
    for (;;) {                                                                 \
      if (current_index >= current_cap) {                                      \
        break;                                                                 \
      }                                                                        \
      int pi = cset__value_pi(cset_vector__index(                              \
          (cset__vector_buckets_ref(cset)), current_index));                   \
      if (pi == 0 || pi == -1) {                                               \
        current_index++;                                                       \
        continue;                                                              \
      }                                                                        \
      cset__add_(cset,                                                         \
                 cset__value_elem_ref(cset_vector__index(                      \
                     (cset__vector_buckets_ref(cset)), current_index)),        \
                 (&((cset)->temp_buckets)),                                    \
                 cset__value_elem(cset_vector__index(                          \
                     (cset__vector_buckets_ref(cset)), current_index)));       \
      current_index++;                                                         \
    }                                                                          \
    cset_vector__free(&((cset)->buckets));                                     \
    ((cset)->buckets) = ((cset)->temp_buckets);                                \
  } while (0)

#define cset__remove_(cset, ref, value, vector_ref)                            \
  do {                                                                         \
    cset__u64 h1 = cset__h1hash(cset, ref, value);                             \
    bool found = false;                                                        \
    size_t iteration = 1;                                                      \
    size_t index;                                                              \
    size_t cap = cset_vector__cap(vector_ref);                                 \
    for (;;) {                                                                 \
      if (((iteration)-1) >= cap)                                              \
        break;                                                                 \
      cset__u64 h2 = cset__h2hash(cset, ref, value);                           \
      index = cset__double_hash_index(h1, h2, (iteration - 1), cap);           \
      iteration++;                                                             \
      if (cset__tombstone(vector_ref, index)) {                                \
        continue;                                                              \
      }                                                                        \
      if (cset__empty(vector_ref, index)) {                                    \
        break;                                                                 \
      }                                                                        \
      bool matches = cset__matches(cset, vector_ref, ref, value, index);       \
      if (matches) {                                                           \
        found = true;                                                          \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    if (found) {                                                               \
      ((cset_vector__index(vector_ref, index))->pi) = -1;                      \
      cset__set_size(cset, (cset__size(cset)) - 1);                            \
    }                                                                          \
  } while (0)

#define cset__remove(cset, value)                                              \
  do {                                                                         \
    ((cset)->v) = value;                                                       \
    cset__remove_(cset, &((cset)->v), ((cset)->v),                             \
                  cset__vector_buckets_ref(cset));                             \
  } while (0)

#define cset__clear(cset)                                                      \
  do {                                                                         \
    cset__free(cset);                                                          \
    cset_vector__init_with_cap(cset__vector_buckets_ref(cset),                 \
                               cset__INITIAL_CAP);                             \
    cset__set_size(cset, 0);                                                   \
                                                                               \
    if (CSET__FORCE_INITIALIZE) {                                              \
      for (size_t i = 0; i < cset_vector__cap(cset__vector_buckets_ref(cset)); \
           i++) {                                                              \
        ((cset_vector__index(cset__vector_buckets_ref(cset), i))->pi) = 0;     \
      }                                                                        \
    }                                                                          \
  } while (0)

#define cset__free(cset) cset_vector__free(cset__vector_buckets_ref(cset))

#define cset__intersect(cset_res, cset_a, cset_b)                              \
  do {                                                                         \
    for (size_t i = 0; i < cset__cap(cset_a); i++) {                           \
      int pi = cset__value_pi(                                                 \
          cset_vector__index((cset__vector_buckets_ref(cset_a)), i));          \
      if (pi == 0 || pi == -1) {                                               \
        continue;                                                              \
      }                                                                        \
      bool contains = false;                                                   \
      cset__contains((cset_b),                                                 \
                     cset__value_elem((cset_vector__index(                     \
                         cset__vector_buckets_ref(cset_a), i))),               \
                     (&contains));                                             \
      if (contains) {                                                          \
        cset__add(cset_res, cset__value_elem((cset_vector__index(              \
                                cset__vector_buckets_ref(cset_a), i))));       \
      }                                                                        \
    }                                                                          \
  } while (0)

#define cset__union(cset_res, cset_a, cset_b)                                  \
  do {                                                                         \
    for (size_t i = 0; i < cset__cap(cset_a); i++) {                           \
      int pi = cset__value_pi(                                                 \
          cset_vector__index((cset__vector_buckets_ref(cset_a)), i));          \
      if (pi == 0 || pi == -1) {                                               \
        continue;                                                              \
      }                                                                        \
      cset__add(cset_res, cset__value_elem((cset_vector__index(                \
                              cset__vector_buckets_ref(cset_a), i))));         \
    }                                                                          \
    for (size_t i = 0; i < cset__cap(cset_b); i++) {                           \
      int pi = cset__value_pi(                                                 \
          cset_vector__index((cset__vector_buckets_ref(cset_b)), i));          \
      if (pi == 0 || pi == -1) {                                               \
        continue;                                                              \
      }                                                                        \
      cset__add(cset_res, cset__value_elem((cset_vector__index(                \
                              cset__vector_buckets_ref(cset_b), i))));         \
    }                                                                          \
  } while (0)

#define cset__is_disjoint(cset_a, cset_b, flag)                                \
  do {                                                                         \
    (*(flag)) = true;                                                          \
    for (size_t i = 0; i < cset__cap(cset_a); i++) {                           \
      int pi = cset__value_pi(                                                 \
          cset_vector__index((cset__vector_buckets_ref(cset_a)), i));          \
      if (pi == 0 || pi == -1) {                                               \
        continue;                                                              \
      }                                                                        \
      bool cset__contains_ = false;                                            \
      cset__contains((cset_b),                                                 \
                     cset__value_elem((cset_vector__index(                     \
                         cset__vector_buckets_ref(cset_a), i))),               \
                     (&cset__contains_));                                      \
      if (cset__contains_) {                                                   \
        (*(flag)) = false;                                                     \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  } while (0)

#define cset__difference(result, cset_a, cset_b)                               \
  do {                                                                         \
    for (size_t i = 0; i < cset__cap(cset_a); i++) {                           \
      int pi = cset__value_pi(                                                 \
          cset_vector__index((cset__vector_buckets_ref(cset_a)), i));          \
      if (pi == 0 || pi == -1) {                                               \
        continue;                                                              \
      }                                                                        \
      bool cset__contains_ = false;                                            \
      cset__contains((cset_b),                                                 \
                     cset__value_elem((cset_vector__index(                     \
                         cset__vector_buckets_ref(cset_a), i))),               \
                     (&cset__contains_));                                      \
      if (!cset__contains_) {                                                  \
        cset__add((result), cset__value_elem((cset_vector__index(              \
                                cset__vector_buckets_ref(cset_a), i))));       \
      }                                                                        \
    }                                                                          \
  } while (0)

#define Cset_iterator(cset_type_)                                              \
  typedef struct cset_iterator_##cset_type_ {                                  \
    cset_type_ *c;                                                             \
    size_t current_count;                                                      \
    size_t current_index;                                                      \
  }

#define cset_iterator__init(iterator, cset)                                    \
  do {                                                                         \
    ((iterator)->c) = (cset);                                                  \
    ((iterator)->current_count) = 0;                                           \
    ((iterator)->current_index) = 0;                                           \
  } while (0)

#define cset_iterator__done(iterator)                                          \
  (((iterator)->current_count) >= ((cset__size(((iterator)->c)))))

#define cset_iterator__next(iterator, buffer)                                  \
  do {                                                                         \
    for (;;) {                                                                 \
      int pi = cset__value_pi(                                                 \
          cset_vector__index((cset__vector_buckets_ref(((iterator)->c))),      \
                             (iterator)->current_index));                      \
      if (pi == 0 || pi == -1) {                                               \
        ((iterator)->current_index)++;                                         \
        continue;                                                              \
      }                                                                        \
      (buffer) = cset__value_elem_ref(                                         \
          cset_vector__index((cset__vector_buckets_ref(((iterator)->c))),      \
                             (iterator)->current_index));                      \
      ((iterator)->current_count)++;                                           \
      ((iterator)->current_index)++;                                           \
      break;                                                                   \
    }                                                                          \
  } while (0)

/* Vector required by cset */

#define cset_Vector(Type_)                                                     \
  typedef struct {                                                             \
    Type_ *e;                                                                  \
    size_t size;                                                               \
    size_t cap;                                                                \
    bool initialized;                                                          \
  }

#define cset_vector__init_with_cap(tv, ncap)                                   \
  do {                                                                         \
    ((tv)->size) = 0;                                                          \
    ((tv)->e) = NULL;                                                          \
    ((tv)->cap) = 0;                                                           \
    cset_vector__grow(tv, ncap);                                               \
    ((tv)->initialized) = true;                                                \
  } while (0)

#define cset_vector__grow(tv, ncap)                                            \
  do {                                                                         \
    size_t cap = ((tv)->cap);                                                  \
    void *mem = malloc(sizeof(*((tv)->e)) * ncap);                             \
    memcpy(mem, ((tv)->e), sizeof(*((tv)->e)) * cap);                          \
    free((tv)->e);                                                             \
    ((tv)->e) = mem;                                                           \
    ((tv)->cap) = ncap;                                                        \
  } while (0)

#define cset_vector__add(tv, value)                                            \
  do {                                                                         \
    size_t size = ((tv)->size);                                                \
    size_t cap = ((tv)->cap);                                                  \
    if (size >= cap) {                                                         \
      size_t new_cap = (cap == 0) ? 1 : (cap * 2);                             \
      cset_vector__grow(tv, new_cap);                                          \
    }                                                                          \
    (((tv)->e)[size]) = value;                                                 \
    ((tv)->size)++;                                                            \
  } while (0)

#define cset_vector__index(tv, i) &(((tv)->e)[i])

#define cset_vector__cap(tv) ((tv)->cap)

#define cset_vector__free(tv) (free((tv)->e))

#endif /* CSET_H */

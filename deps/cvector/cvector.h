#ifndef cvector_h
#define cvector_h

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define CVector(cvector__elem_type_)                                                               \
  typedef struct {                                                                                 \
    cvector__elem_type_ *cvector__elem_m;                                                          \
    size_t cvector__size_m;                                                                        \
    size_t cvector__cap_m;                                                                         \
    bool cvector__initialized_m;                                                                   \
  }

#ifndef CVECTOR__MIN_SHRINK_SIZE
#define CVECTOR__MIN_SHRINK_SIZE 32
#endif

#ifndef CVECTOR__LOAD_FACTOR
#define CVECTOR__LOAD_FACTOR 0.25
#endif

#define cvector__elem_(vec) ((vec)->cvector__elem_m)
#define cvector__set_elem_(vec, elem) ((cvector__elem_(vec)) = (elem))
#define cvector__elem_size_(vec) (sizeof((*(cvector__elem_(vec)))))
#define cvector__size(vec) ((vec)->cvector__size_m)
#define cvector__setsize_(vec, size) ((cvector__size(vec) = (size)))
#define cvector__cap_(vec) ((vec)->cvector__cap_m)
#define cvector__setcap_(vec, cap) (cvector__cap_(vec) = (cap))
#define cvector__initialized_(vec) ((vec)->cvector__initialized_m)
#define cvector__set_initialized_(vec, flag) (cvector__initialized_(vec) = (flag))

#define cvector__init(vec)                                                                         \
  do {                                                                                             \
    cvector__setcap_((vec), 0);                                                                    \
    cvector__setsize_((vec), 0);                                                                   \
    cvector__set_elem_((vec), (NULL));                                                             \
    cvector__set_initialized_((vec), true);                                                        \
  } while (0)

#define cvector__init_with_cap(vec, cap)                                                           \
  do {                                                                                             \
    cvector__setsize_((vec), 0);                                                                   \
    cvector__setcap_((vec), 0);                                                                    \
    cvector__set_elem_((vec), (NULL));                                                             \
    cvector__resize_((vec), (cap));                                                                \
    cvector__set_initialized_((vec), true);                                                        \
  } while (0)

#define cvector__resize_(vec, cap)                                                                 \
  do {                                                                                             \
    void *cvector__mem_m = malloc((cvector__elem_size_(vec)) * (cap));                             \
    memcpy((cvector__mem_m), (cvector__elem_(vec)),                                                \
           ((cvector__elem_size_(vec)) * (cvector__size(vec))));                                   \
    free(cvector__elem_(vec));                                                                     \
    cvector__set_elem_((vec), (cvector__mem_m));                                                   \
    cvector__setcap_((vec), (cap));                                                                \
  } while (0)

#define cvector__add(vec, val)                                                                     \
  do {                                                                                             \
    if (cvector__size(vec) >= cvector__cap_(vec)) {                                                \
      cvector__resize_((vec), ((cvector__cap_(vec) == 0) ? 1 : ((cvector__cap_(vec)) * 2)));       \
    }                                                                                              \
    (((cvector__elem_(vec))[cvector__size(vec)]) = (val));                                         \
    cvector__setsize_((vec), (cvector__size(vec) + 1));                                            \
  } while (0)

#define cvector__set_at_index(vec, index, val)                                                     \
  (((index) < cvector__size(vec)) ? (((cvector__elem_(vec))[(index)]) = (val), 0) : -1)

#define cvector__index(vec, index) (&((cvector__elem_(vec))[(index)]))
#define cvector__index_cpy(vec, index) ((cvector__elem_(vec))[(index)])

#define cvector__free(vec)                                                                         \
  do {                                                                                             \
    free(cvector__elem_(vec));                                                                     \
    cvector__set_elem_((vec), NULL);                                                               \
  } while (0)

#define cvector__shrink_(vec)                                                                      \
  do {                                                                                             \
    double cvector__current_load_factor =                                                          \
        (((double)cvector__size(vec)) / ((double)cvector__cap_(vec)));                             \
    if ((cvector__current_load_factor <= CVECTOR__LOAD_FACTOR) &&                                  \
        (cvector__size(vec) >= CVECTOR__MIN_SHRINK_SIZE)) {                                        \
      cvector__resize_((vec), (cvector__size(vec) * 2));                                           \
    }                                                                                              \
  } while (0)

#define cvector__pop(vec)                                                                          \
  ({                                                                                               \
    cvector__shrink_(vec);                                                                         \
    cvector__setsize_((vec), (cvector__size(vec) - 1));                                            \
    (cvector__index((vec), cvector__size(vec)));                                                   \
  })

#define cvector__pop_cpy(vec)                                                                      \
  ({                                                                                               \
    cvector__shrink_(vec);                                                                         \
    cvector__setsize_((vec), (cvector__size(vec) - 1));                                            \
    (cvector__index_cpy((vec), cvector__size(vec)));                                               \
  })

#define cvector__first(vec) (cvector__index((vec), 0))
#define cvector__first_cpy(vec) (cvector__index_cpy((vec), 0))

#define cvector__last(vec) (cvector__index((vec), cvector__size(vec) - 1))
#define cvector__last_cpy(vec) (cvector__index_cpy((vec), cvector__size(vec) - 1))

#define cvector__wrapped_buffer(vec) cvector__elem_(vec)

#define CVector_iterator(cvector__type_)                                                           \
  typedef struct {                                                                                 \
    cvector__type_ *cvector_iterator__vec_m;                                                       \
    size_t cvector_iterator__current_index_m;                                                      \
  }

#define cvector_iterator__vec_(iterator) ((iterator)->cvector_iterator__vec_m)
#define cvector_iterator__set_vec_(iterator, vec) ((cvector_iterator__vec_(iterator)) = (vec))

#define cvector_iterator__current_index_(iterator) ((iterator)->cvector_iterator__current_index_m)
#define cvector_iterator__set_current_index_(iterator, index)                                      \
  ((cvector_iterator__current_index_(iterator)) = (index))

#define cvector_iterator__init(iterator, vec)                                                      \
  do {                                                                                             \
    cvector_iterator__set_vec_((iterator), (vec));                                                 \
    cvector_iterator__set_current_index_((iterator), 0);                                           \
  } while (0)

#define cvector_iterator__done(iterator)                                                           \
  (((cvector_iterator__vec_(iterator)) != (NULL))                                                  \
       ? ((cvector_iterator__current_index_(iterator)) >=                                          \
          cvector__size(cvector_iterator__vec_(iterator)))                                         \
       : (true))

#define cvector_iterator__next(iterator)                                                           \
  (cvector__index((cvector_iterator__vec_(iterator)),                                              \
                  (((cvector_iterator__current_index_(iterator))++))))

#define cvector_iterator__next_cpy(iterator)                                                       \
  (cvector__index_cpy((cvector_iterator__vec_(iterator)),                                          \
                      (((cvector_iterator__current_index_(iterator))++))))

#define cvector_iterator__peek(iterator)                                                           \
  (cvector__index((cvector_iterator__vec_(iterator)), (cvector_iterator__current_index_(iterator))))

#define cvector_iterator__peek_cpy(iterator)                                                       \
  (cvector__index_cpy((cvector_iterator__vec_(iterator)),                                          \
                      (cvector_iterator__current_index_(iterator))))

#define cvector_iterator__wrapped_iterable(iterator) ((cvector_iterator__vec_(iterator)))

#define cvector_iterator__reset(iterator) ((cvector_iterator__set_current_index_((iterator), 0)))

#endif /* cvector_h */

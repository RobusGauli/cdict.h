#include <assert.h>
#include <stdio.h>

#include "deps/cset/cset.h"
#include "deps/cvector/cvector.h"
#include "src/cdict.h"

void test__cdict_add() {
  CDict(int, int) cdict_int_int_t;
  cdict_int_int_t cdict;
  cdict__init(&cdict);

  for (size_t i = 0; i < 1000; i++) {
    cdict__add(&cdict, i, i * i);
  }
  assert(cdict__size(&cdict) == 1000);

  cdict__free(&cdict);
}

void test__cdict_remove() {
  typedef struct {
    int x;
    int y;
  } Node;

  CDict(int, Node) cdict_t;

  cdict_t cdict;
  cdict__init(&cdict);

  cdict__add(&cdict, 4, ((Node){.x = 2, .y = 34}));
  cdict__add(&cdict, 6, ((Node){.x = 23, .y = 123}));

  assert(cdict__size(&cdict) == 2);

  bool removed = cdict__remove(&cdict, 4);
  assert(removed);
  assert(cdict__size(&cdict) == 1);

  Node node;

  bool ok = cdict__get(&cdict, 4, &node);
  assert(ok == false);

  {
    bool ok = cdict__get(&cdict, 6, &node);
    assert(ok);
    assert(node.x == 23);
    assert(node.y == 123);
  }

  cdict__free(&cdict);
}

void test__cdict_init() {
  CDict(int, int) cdict_t;
  cdict_t cdict;

  cdict__init(&cdict);

  assert(cdict__size(&cdict) == 0);
  assert(cdict__vector_buckets_ref(&cdict) != NULL);
  assert(cdict_vector__cap(cdict__vector_buckets_ref(&cdict)) ==
         CDICT__INITIAL_CAP);
  assert(cdict__seed(&cdict) == CDICT__DEFAULT_SEED);
  assert(cdict__max_load_factor(&cdict) == CDICT__MAX_LOAD_FACTOR);
  assert(cdict__min_load_factor(&cdict) == CDICT__MIN_LOAD_FACTOR);
  assert(cdict.cdict__hash_m == NULL);
  assert(cdict.cdict__compare_m == NULL);

  cdict__free(&cdict);
}

void test__cdict_clear() {
  CDict(int, int) cdict_t;
  cdict_t cdict;

  cdict__init(&cdict);

  cdict__add(&cdict, 1, 2);
  cdict__add(&cdict, 2, 3);
  assert(cdict__size(&cdict) == 2);

  cdict__clear(&cdict);
  assert(cdict__size(&cdict) == 0);

  cdict__free(&cdict);
}

void test__cdict_free() {
  CDict(int, int) cdict_t;

  cdict_t cdict;

  cdict__init(&cdict);

  assert((cdict_vector__elem((cdict__vector_buckets_ref(&cdict)))) != NULL);
  cdict__free(&cdict);
  assert((cdict_vector__elem((cdict__vector_buckets_ref(&cdict)))) == NULL);
}

void test__cdict_get() {
  CDict(char, int) cdict_t;
  cdict_t cdict;
  cdict__init(&cdict);

  cdict__add(&cdict, 'a', 10);
  cdict__add(&cdict, 'b', 12);

  int buffer;

  bool ok = cdict__get(&cdict, 'c', &buffer);
  assert(ok == false);

  {
    bool ok = cdict__get(&cdict, 'b', &buffer);
    assert(ok);
    assert(buffer == 12);
  }

  cdict__free(&cdict);
}

void test__cdict_contains() {
  typedef char *string;
  CDict(string, string) cdict_t;

  cdict_t cdict;

  cdict__init(&cdict);

  cdict__add(&cdict, "name", "robus");
  cdict__add(&cdict, "age", "12");

  bool contains = cdict__contains(&cdict, "home");
  assert(contains == false);

  {
    bool contains = cdict__contains(&cdict, "name");
    assert(contains);
  }

  cdict__free(&cdict);
}

void test__cdict_size() {
  typedef char *string;
  CDict(string, string) cdict_t;

  cdict_t cdict;
  cdict__init(&cdict);

  assert(cdict__size(&cdict) == 0);
  cdict__add(&cdict, "name", "robus");
  assert(cdict__size(&cdict) == 1);

  cdict__add(&cdict, "name", "rahul");
  assert(cdict__size(&cdict) == 1);

  cdict__add(&cdict, "address", "kathmandu");
  assert(cdict__size(&cdict) == 2);

  bool ok = cdict__remove(&cdict, "name");
  assert(ok);
  assert(cdict__size(&cdict) == 1);

  {
    bool ok = cdict__remove(&cdict, "random");
    assert(ok == false);
    assert(cdict__size(&cdict) == 1);
  }

  cdict__clear(&cdict);
  assert(cdict__size(&cdict) == 0);
  cdict__free(&cdict);
}

void test__cdict_iteration() {
  CDict(int, int) cdict_t;
  cdict_t cdict;

  cdict__init(&cdict);

  for (size_t i = 0; i < 100; i++) {
    cdict__add(&cdict, i, i * i);
  }

  assert(cdict__size(&cdict) == 100);

  CDict_iterator(cdict_t) iterator_t;

  iterator_t iterator;
  cdict_iterator__init(&iterator, &cdict);

  for (;;) {
    if (cdict_iterator__done(&iterator))
      break;

    int key = cdict_iterator__next(&iterator);
    int value;

    cdict__get(&cdict, key, &value);
    assert(key * key == value);
  }
  cdict__free(&cdict);
}

#define cdict_size 10
void test__cdict_fromkeys() {
  CDict(int, int) cdict_t;
  cdict_t cdict;

  cdict__init(&cdict);
  int keys[cdict_size] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  cdict__fromkeys(&cdict, keys, cdict_size, 1);

  assert(cdict__size(&cdict) == 10);

  int buffer;
  bool ok = cdict__get(&cdict, 1, &buffer);
  assert(ok);
  cdict__free(&cdict);
}

void test__cdict_pop() {
  typedef char *string;
  CDict(string, int) cdict_t;

  cdict_t cdict;

  cdict__init(&cdict);

  cdict__add(&cdict, "one", 1);
  cdict__add(&cdict, "two", 2);
  cdict__add(&cdict, "three", 3);

  assert(cdict__size(&cdict) == 3);

  int buffer;
  bool ok = cdict__pop(&cdict, "one", &buffer);

  assert(buffer == 1);
  assert(ok);

  {
    bool ok = cdict__pop(&cdict, "random", &buffer);
    assert(ok == false);
  }
  assert(cdict__size(&cdict) == 2);
}

void test__cdict_values_iteration() {
  Cset(int) cset_int_t;
  cset_int_t cset_int;
  cset__init(&cset_int);

  CDict(int, int) cdict_t;

  cdict_t cdict;

  cdict__init(&cdict);

  cdict__add(&cdict, 3, 1);
  cdict__add(&cdict, 4, 5);
  cdict__add(&cdict, 6, 7);

  cset__add(&cset_int, 1);
  cset__add(&cset_int, 5);
  cset__add(&cset_int, 7);

  CDict_iterator(cdict_t) iterator_t;
  iterator_t iterator;
  cdict_iterator__init(&iterator, &cdict);

  for (;;) {
    if (cdict_iterator__done(&iterator))
      break;

    int value = cdict_iterator__next_val(&iterator);

    bool contains;
    cset__contains(&cset_int, (value), &contains);
    assert(contains);
  }

  cset__free(&cset_int);
  cdict__free(&cdict);
}

void test__copy_keys_to_vector() {

  CDict(int, int) cdict_t;
  cdict_t cdict;

  cdict__init(&cdict);

  CVector(int) vector_int_t;
  vector_int_t vector_int;
  cvector__init(&vector_int);

  for (size_t i = 0; i < 100; i++) {
    cdict__add(&cdict, i, i);
  }

  CDict_iterator(cdict_t) iterator_t;
  iterator_t iterator;
  cdict_iterator__init(&iterator, &cdict);

  for (;;) {
    if (cdict_iterator__done(&iterator))
      break;

    int key = cdict_iterator__next(&iterator);
    cvector__add(&vector_int, key);
  }

  assert(cvector__size(&vector_int) == 100);
}

typedef struct {
  int x;
  int y;
} Node_t;

bool node_comparator(Node_t *self, Node_t *other) {
  return ((self->x) == (other->x));
}

cdict__u64 node_hasher(Node_t *self, cdict__u64 (*hash)(void *, size_t)) {
  return hash(&(self->x), sizeof((self)->x));
}

void test__custom_comparator_hasher() {

  CDict(Node_t, int) cdict_node_t;
  cdict_node_t cdict_node;

  cdict__init(&cdict_node);
  cdict__set_comparator(&cdict_node, node_comparator);
  cdict__set_hash(&cdict_node, node_hasher);

  cdict__add(&cdict_node, ((Node_t){.x = 3, .y = 34}), 34);
  cdict__add(&cdict_node, ((Node_t){.x = 3, .y = 35}), 10);

  assert(cdict__size(&cdict_node) == 1);

  cdict__add(&cdict_node, ((Node_t){.x = 12, .y = 34}), 100);
  cdict__add(&cdict_node, ((Node_t){.x = 16, .y = 34}), 100);

  assert(cdict__size(&cdict_node) == 3);
}

void test__cdict_keyval_iteration() {
  CDict(int, int) cdict_int_t;
  cdict_int_t cdict_int;

  cdict__init(&cdict_int);

  for (size_t i = 0; i < 1000; i++) {
    cdict__add(&cdict_int, i, i * i);
  }

  CDict_iterator(cdict_int_t) cdict_iterator_t;
  cdict_iterator_t iterator;

  cdict_iterator__init(&iterator, &cdict_int);

  for (;;) {
    if (cdict_iterator__done(&iterator))
      break;

    int value;
    int key = cdict_iterator__next_keyval(&iterator, &value);
    assert(key * key == value);
  }

  cdict__free(&cdict_int);
}

void test__cdict_struct_iteration() {

  typedef struct {
    int x;
    int y;
  } Node;

  CDict(int, Node) cdict_node_t;
  cdict_node_t cdict_node;
  cdict__init(&cdict_node);

  Node node_a = {.x = 1, .y = 2};
  Node node_b = {.x = 3, .y = 4};
  Node node_c = {.x = 5, .y = 6};

  cdict__add(&cdict_node, 1, node_a);
  cdict__add(&cdict_node, 2, node_b);
  cdict__add(&cdict_node, 3, node_c);

  Node node;
  bool ok = cdict__get(&cdict_node, 2, &node);
  assert(ok && node.x == 3 && node.y == 4);

  CDict_iterator(cdict_node_t) cdict_iterator_t;
  cdict_iterator_t cdict_iterator;

  cdict_iterator__init(&cdict_iterator, &cdict_node);

  CVector(Node) vector_node_t;
  vector_node_t vector_node;
  cvector__init(&vector_node);

  for (;;) {
    if (cdict_iterator__done(&cdict_iterator))
      break;

    Node value = cdict_iterator__next_val(&cdict_iterator);
    cvector__add(&vector_node, value);
  }

  assert(cvector__size(&vector_node) == 3);
}

void test__cdict_nested_cdict() {

  typedef char *string;
  CDict(int, string) cdict_string_t;
  CDict(int, cdict_string_t) cdict_t;

  CDict_iterator(cdict_t) cdict_iterator_t;
  CDict_iterator(cdict_string_t) cdict_string_iterator_t;

  cdict_t cdict;
  cdict__init(&cdict);

  {
    cdict_string_t cdict_string;
    cdict__init(&cdict_string);
    cdict__add(&cdict_string, 1, "one");
    cdict__add(&cdict_string, 2, "two");
    cdict__add(&cdict_string, 3, "three");

    cdict__add(&cdict, 1, cdict_string);
  }

  {
    cdict_string_t cdict_string;
    cdict__init(&cdict_string);
    cdict__add(&cdict_string, 4, "four");
    cdict__add(&cdict_string, 5, "five");
    cdict__add(&cdict_string, 6, "six");

    cdict__add(&cdict, 2, cdict_string);
  }

  assert(cdict__size(&cdict) == 2);

  {
    cdict_string_t cdict_string;
    bool ok = cdict__get(&cdict, 1, &cdict_string);
    assert(ok);
    assert(cdict__size(&cdict_string) == 3);

    {
      string value;
      bool ok = cdict__get(&cdict_string, 1, &value);
      assert(ok);
      assert(strcmp(value, "one") == 0);
    }

    {
      string value;
      bool ok = cdict__get(&cdict_string, 2, &value);
      assert(ok);
      assert(strcmp(value, "two") == 0);
    }

    {
      string value;
      bool ok = cdict__get(&cdict_string, 3, &value);
      assert(ok);
      assert(strcmp(value, "three") == 0);
    }

    {
      string value;
      bool ok = cdict__get(&cdict_string, 4, &value);
      assert(!ok);
    }
  }

  {
    cdict_iterator_t cdict_iterator;
    cdict_iterator__init(&cdict_iterator, &cdict);

    CVector(cdict_string_t) vector_t;
    vector_t vector;
    cvector__init(&vector);

    for (;;) {
      if (cdict_iterator__done(&cdict_iterator))
        break;

      cdict_string_t cdict_string = cdict_iterator__next_val(&cdict_iterator);
      cvector__add(&vector, cdict_string);
    }

    assert(cvector__size(&vector) == 2);
  }
}

int main() {
  test__cdict_init();
  test__cdict_add();
  test__cdict_remove();
  test__cdict_clear();
  test__cdict_free();
  test__cdict_size();
  test__cdict_iteration();
  test__cdict_values_iteration();
  test__cdict_keyval_iteration();
  test__cdict_struct_iteration();
  test__cdict_nested_cdict();
  test__cdict_get();
  test__cdict_contains();
  test__cdict_fromkeys();
  test__cdict_pop();
  test__copy_keys_to_vector();
  test__custom_comparator_hasher();
}

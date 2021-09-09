
# CDict(T)
### Typesafe, Generic, and Extremely Fast Dictionary/Hashmap implementation in C 🚀


### Key Features
* Extremely fast non-cryptographic hash algorithm [XXHash](https://cyan4973.github.io/xxHash/)
* Complete Typesafe APIs
* **[Double Hashing](https://en.wikipedia.org/wiki/Double_hashing)** to avoid both Primary & Secondary clustering
* **[Robinhood Hash](https://www.cs.cornell.edu/courses/JavaAndDS/files/hashing_RobinHood.pdf)**(*in progress*) for near constant time access
* Iterators for streaming usecase
* Allows **Custom Hash** & **Custom Comparator**
* Single header file to rule them all 🚀 🚀 🚀

### Installation
Simply copy paste `cdict.h` in your source directly or you can use [Clib](https://github.com/clibs/clib) package manager for installation
```sh
clib install robusgauli/cdict.h
```
### Getting Started
```c
#include <stdio.h>
#include "cdict.h"

// Declare CDict of [int:int] type
CDict(int, int) cdict_int_t;
// Declare CDict iterator of `cdict_int_t` type
CDict_iterator(cdict_int_t) cdict_iterator_int_t;

int main() {
    // Instance of cdict_int_t;
    cdict_int_t cdict_int;

    // Initialize
    cdict__init(&cdict_int);

    // Add elements
    cdict__add(&cdict_int, 1, 100);
    cdict__add(&cdict_int, 2, 200);
    cdict__add(&cdict_int, 2, 300); // Duplicate

    size_t size = cdict__size(&cdict_int);
    printf("Size of dictionary is: %ld\n", size);

    // Iteration
    cdict_iterator_int_t cdict_iterator_int;

    // Initialize iterator with cdict_int;
    cdict_iterator__init(&cdict_iterator_int, &cdict_int);

    // Traverse
    for(;;) {
      if (cdict_iterator__done(&cdict_iterator_int)) break;

      int key = cdict_iterator__next(&cdict_iterator_int);

      printf("Got key: %d\n", key);
    }

    cdict__free(&cdict_int);
}
```


### APIs

* `cdict__add(cdict, key, value)`: *no return* <br />

Add key/value pair to dictionary
<br />

```c
#include "cdict.h"

CDict(int, char) cdict_int_char_t;

int main() {
  cdict_int_char_t cdict;
  cdict__init(&cdict);

  // Add key/val pair to dictionary
  cdict__add(&cdict, 1, '1');
  cdict__add(&cdict, 2, '2');
  cdict__add(&cdict, 3, '3');

  // Free up the heap allocated resource
  cdict__free(&cdict);
}
```

* `cdict__remove(cdict, key)`: *returns bool* <br />


Removes key/value pair from the dictionary given key as an argument. Returns `true` if successfully removed from the dictionary. <br/>

```c
#include "cdict.h"
#include <assert.h>

typedef char* string_t;

CDict(string_t, string_t) cdict_string_string_t;
int main() {
  cdict_string_string_t cdict;
  cdict__init(&cdict);

  cdict__add(&cdict, "firstname", "alan");
  cdict__add(&cdict, "lastname", "turing");

  // Remove key/value pair by `key`
  {
    bool ok = cdict__remove(&cdict, "firstname");
    assert(ok);
  }

  // Try removing with wrong key
  {
    bool ok = cdict__remove(&cdict, "blaaaaaaaaaah");
    assert(!ok);
  }

  cdict__free(&cdict);
}
```

* `cdict__size(cdict)`: *returns `size_t`*

Returns the size of dictionary.

```c
#include "cdict.h"
#include <assert.h>

CDict(int, int) cdict_int_int_t;

int main() {
  cdict_int_int_t cdict;
  cdict__init(&cdict);

  cdict__add(&cdict, 1, 12);
  cdict__add(&cdict, 2, 12);
  cdict__add(&cdict, 3, 24);

  // Size of the dictionary
  size_t size = cdict__size(&cdict);
  printf("Size of Dictionary: %ld\n", size);

  cdict__free(&cdict);
}
```


* `cdict__clear(cdict)`: *no return*
Removes all the key/value pairs from the dictionary.

```c
#include <assert.h>
#include "cdict.h"

typedef char* string;
CDict(string, int) cdict_string_int_t;

int main() {
  cdict_string_int_t cdict;
  cdict__init(&cdict);

  cdict__add(&cdict, "one", 1);
  cdict__add(&cdict, "two", 2);
  cdict__add(&cdict, "three", 3);

  // Clear dictionary
  cset__clear(&cdict);
  assert(cdict__size(&cdict) == 0);

  cdict__free(&cdict)
}
```

* `cdict__contains(cdict, key)`: *returns `bool`* <br/>

Test whether an key is in dictionary.
```c
#include <assert.h>
#include "cdict.h"

CDict(int, double) cdict_int_double_t;

int main() {
  cdict_int_double_t cdict;
  cdict__init(&cdict);

  cdict__add(&cdict, 1, 1.1);
  cdict__add(&cdict, 2, 2.2);

  {
    // Positive membership test
    bool ok = cdict__contains(&cdict, 1);
    assert(ok);
  }

  {
    // Negative membership test
    bool ok = cdict__contains(&cdict, 4123);
    assert(!ok);
  }

  cdict__free(&cdict);
}
```
* `cdict__set_comparator` & `cdict__set_hash()`: *no return* <br/>

   **NOTE:** Both **Custom Comparator and Hash** must be implemented.

    These above methods allows us to use custom hashing and comparator for complex structs. Please go through this [link](https://stackoverflow.com/questions/2265503/why-do-i-need-to-override-the-equals-and-hashcode-methods-in-java) for why we need to implement both functions for correctness.

```c
#include <assert.h>
#include "cdict.h"

typedef struct {
  int x;
  int y;
} Node;

// Custom comparator
bool custom_comparator(Node* self, Node* other) {
  return (self -> x) == (other -> x);
}
// Custom hash function
// It takes pointer to Node and function as an arguments
cdict__u64 custom_hash(Node* self, cdict__u64 (*hash)(void*, size_t)) {
  // VVIP: Hash function requires pointer to data and the size in bytes
  return hash(&(self -> x), sizeof(self -> x));
}

CDict(Node, int) cdict_node_t;

int main() {
  cdict_node_t cdict_node;
  cdict__init(&cdict_node);
  cdict__set_hash(&cdict_node, custom_hash);
  cdict__set_comparator(&cdict_node, custom_comparator);

  cdict__add(&cdict_node, ((Node){.x=4,.y=5}), 1);
  // Duplicate because our custom comparator and hash uses `x`
  // instead of whole struct for hash
  cdict__add(&cdict_node, ((Node){.x=4,.y=4}), 234);

  assert(cdict__size(&cdict_node) == 1);

  cdict__add(&cdict_node, ((Node){.x=1, .y=2}), 12);
  assert(cdict__size(&cdict_node) == 2);

  // Removed because our comparator uses `x` i.e 1 which already exists
  bool ok = cdict__remove(&cdict__node, ((Node){.x=1, .y=45}));
  assert(ok);
  assert(cdict__size(&cdict__node) == 1);

  cdict__free(&cdict_node);
}
```

* `cdict__free`: *no return* <br/>
Frees up heap allocation

```c
#include "cdict.h"

CDict(int, int) cdict_t;

int main() {
  cdict_t cdict;
  cdict__init(&cdict);

  cdict__add(&cdict, 1, 2);

  // Free up heap mem
  cdict__free(&cdict);
}
```

### APIS for Iteration

* `Cset_iterator(type)`
    Creates Iterator type definition for a given cset type.

* `cset_iterator__init(iter, cset)`: *no return*
    Initializes iterator `iter` with `cset`.
  ```c
  #include "cset.h"

  Cset(int) cset_int_t;
  // 1. Define iterator type for `cset_int_t`
  Cset_iterator(cset_int_t) cset_iterator_int_t;

  int main() {
    cset_int_t cset_int;
    cset__init(&cset_int);

    cset_iterator_int_t cset_int_iterator;
    // 2. Initialize iterator with `cset_int`
    cset_iterator__init(&cset_int_iterator, &cset_int);
  }
  ```

* `cset__done(iter)`: *returns `bool`*
    Returns whether the iteration is complete or not.
  ```c
  #include <assert.h>
  #include "cset.h"

  Cset(int) cset_int_t;
  Cset_iterator(cset_int_t) cset_iterator_int_t;

  int main() {
    cset_int_t cset_int;
    cset__init(&cset_int);

    cset__add(&cset_int, 34);
    cset__add(&cset_int, 56);

    cset_iterator_int_t cset_int_iterator;
    cset_iterator__init(&cset_int_iterator, &cset_int);

    // Checks whether the iteration is complete or not
    bool done = cset_iterator__done(&cset_int_iterator);
    assert(done == false);
  }
  ```

* `cset__next(iter, buffer)`: *no return*

    Yields pointer to next element into buffer. Note: buffer must be pointer type.

  ```c
  #include <assert.h>
  #include "cset.h"


  Cset(int) cset_int_t;
  Cset_iterator(cset_int_t) cset_iterator_int_t;

  int main() {
    cset_int_t cset_int;
    cset__init(&cset_int);

    cset__add(&cset_int, 34);
    cset__add(&cset_int, 56);

    cset_iterator_int_t cset_int_iterator;
    cset_iterator__init(&cset_int_iterator, &cset_int);

    for (;;) {
      if (cset_iterator__done(&cset_int_iterator)) break;

      // Yields pointer to next element into buffer
      int* buffer;
      cset_iterator__next(&cset_int_iterator, buffer);

      printf("Got: %d\n", *buffer);
    }

    cset__free(&cset_int);
  }
  ```

### APIS for basic set operations

* `cset__intersection(result, a, b)`: *no return*
Puts the intersection of two sets `a` and `b` in `result`.

  ```c
  #include <assert.h>
  #include "cset.h"

  Cset(int) cset_int_t;

  int main() {
    cset_int_t cset_int_a;
    cset_int_t cset_int_b;

    cset__init(&cset_int_a);
    cset__init(&cset_int_b);

    // Add elements to cset_int_a
    cset__add(&cset_int_a, 12);
    cset__add(&cset_int_a, 13);
    cset__add(&cset_int_a, 14);

    // Add elements to cset_int_b
    cset__add(&cset_int_b, 12);
    cset__add(&cset_int_b, 13);
    cset__add(&cset_int_b, 16);

    cset_int_t result;
    cset__init(&result);

    // Perform intersection between cset_int_a and cset_int_b
    cset__intersect(&result, &cset_int_a, &cset_int_b);

    assert(cset__size(&result) == 2);

    cset__free(&cset_int_a);
    cset__free(&cset_int_b);
    cset__free(&result);
  }
  ```

* `cset__union(result, a, b)`: *no return*
Puts all the elements that are in either set into `result`

  ```c
  #include <assert.h>
  #include "cset.h"

  Cset(int) cset_int_t;
  Cset_iterator(cset_int_t) cset_iterator_int_t;

  int main() {
    cset_int_t cset_int_a;
    cset_int_t cset_int_b;
    cset_int_t cset_result;

    cset__init(&cset_int_a);
    cset__init(&cset_int_b);
    cset__init(&cset_result);

    cset__add(&cset_int_a, 34);
    cset__add(&cset_int_a, 25);
    cset__add(&cset_int_a, 12);

    cset__add(&cset_int_b, 1);
    cset__add(&cset_int_b, 4);
    cset__add(&cset_int_b, 34);

    // Perform union between `cset_int_a` and `cset_int_b`
    cset__union(&cset_result, &cset_int_a, &cset_int_b);

    // Size of the `cset_result` set must be 5 with elements { 34, 25, 12, 1, 4 }
    assert(cset__size(&cset_result) == 5);

    cset__add(&cset_int_b, 100);
    cset__union(&cset_result, &cset_int_a, &cset_int_b);
    // Size of the `cset_result` set must be 6 as we added new elements to `cset_int_b`
    assert(cset__size(&cset_result) == 6);

    cset__free(&cset_int_a);
    cset__free(&cset_int_b);
    cset__free(&cset_result);
  }
  ```

### License

Copyright © 2020-20121 Robus, LLC. This source code is licensed under the MIT license found in
the [LICENSE.txt]
The documentation to the project is licensed under the [CC BY-SA 4.0](http://creativecommons.org/licenses/by-sa/4.0/)
license.


---
Made with ♥ by Robus Gauli ([@robusgauli]

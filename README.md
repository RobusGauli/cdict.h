
# CDict(T)
### Typesafe, Generic, and Extremely Fast Dictionary in C 🚀


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

* `cdict__get(cdict, key, buffer)`: *returns bool* <br/>

Get value for a corresponding key from dictionary

```c
#include "cdict.h"

typedef char* string;
CDict(string, string) cdict_string_t;

int main() {
  cdict_string_t cdict_string;
  cdict__init(&cdict_string);

  cdict__add(&cdict_string, "firstname", "Alan");
  cdict__add(&cdict_string, "lastname", "Turing");

  string firstname;
  bool ok =  cdict__get(&cdict_string, "firstname", &firstname);
  if (ok) {
    printf("Firstname is: %s\n", firstname);
  }

  string middlename;
  bool found = cdict__get(&cdict_string, "middlename", &middlename);
  if (!found) {
    printf("Could not find `middlename` as a key in dictionary.\n");
  }

  cdict__free(&cdict_string);
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


* `cdict__clear(cdict)`: *no return <br/>
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

* `CDict_iterator(type)` <br />

Creates Iterator type definition for a given cdict type.

* `cdict_iterator__init(iter, cdict)`: *no return* <br/>

Initializes iterator `iter` with `cdict`.

```c
#include "cdict.h"

CDict(int, int) cdict_int_t;
// 1. Define iterator type for `cdict_int_t`
CDict_iterator(cdict_int_t) cdict_iterator_t;

int main() {
  cdict_int_t cdict_int;
  cdict__init(&cdict_int);

  cdict_iterator_t cdict_iterator;
  // 2. Initialize iterator with `cdict_int`
  cdict_iterator__init(&cdict_iterator, &cdict_int);
}
```

* `cdict_iterator__done(iter)`: *returns `bool`* <br/>

Returns whether the iteration is complete or not.

```c
#include <assert.h>
#include "cdict.h"

CDict(int, int) cdict_int_t;
CDict_iterator(cdict_int_t) cdict_iterator_t;

int main() {
  cdict_int_t cdict;
  cdict__init(&cdict);

  cdict__add(&cdict, 2, 4);
  cdict__add(&cdict, 3, 9);
  cdict__add(&cdict, 4, 16);

  cdict_iterator_t cdict_iterator;
  cdict_iterator__init(&cdict_iterator, &cdict);

  // Checks whether the iteration is complete or not
  bool done = cset_iterator__done(&cdict_iterator);
  assert(done == false);
}
```

* `cset__next(iter, buffer)`: *no return* <br />

Yields next key from the iterator.

```c
#include <assert.h>
#include "cdict.h"


CDict(int, int) cdict_int_t;
CDict_iterator(cdict_int_t) cdict_iterator_t;

int main() {
  cdict_int_t cdict_int;
  cdict__init(&cdict_int);

  cdict__add(&cdict_int, 2, 4);
  cdict__add(&cdict_int, 3, 9);
  cdict__add(&cdict_int, 4, 16);

  cdict_iterator_t cdict_iterator;
  cdict_iterator__init(&cdict_iterator, &cdict_int);

  for (;;) {
    if (cdict_iterator__done(&cdict_iterator)) break;

    int key = cdict_iterator__next(&cdict_iterator);

    printf("Got key: %d\n", key);
  }
  cdict__free(&cdict_int)
}
```

### License

Copyright © 2020-20121 Robus, LLC. This source code is licensed under the MIT license found in
the [LICENSE.txt]
The documentation to the project is licensed under the [CC BY-SA 4.0](http://creativecommons.org/licenses/by-sa/4.0/)
license.


---
Made with ♥ by Robus Gauli ([@robusgauli]

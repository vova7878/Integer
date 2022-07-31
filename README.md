# Integer
Open source library of different size integers

Copyright (c) 2022 Vladimir Kozelkov

Please see LICENSE file for license.

## Usage
This is a simple implementation of integer types in C++. It is intended to be used like the standard uintX_t, except for a different name.

### In Code
All that needs to be done in code is `#include "Integer.hpp"` or `#include "Integer_literal.hpp"` to support integer literals

```c++
#include <iostream>
#include "Integer.hpp"

using int128_t = JIO::Integer<16, true>;
using uint128_t = JIO::Integer<16, false>;

int main(int, char**) {
    int128_t a = 1;
    uint128_t b = 2;
    std::cout << ((a | b) == 3) << std::endl; //true
    return 0;
}
```
## Compilation
A C++ compiler supporting at least C++14 is required.

# Integer
Open source library of different size integers

Copyright (c) 2022 Vladimir Kozelkov

Please see LICENSE file for license.

## Usage
This is a simple implementation of integer types in C++. It is intended to be used like the standard uintX_t, except for a different name.

### In Code
All that needs to be done in code is `#include "Integer.hpp"`

```c++
#include <iostream>
#include "Integer.hpp"

using int128_t = JIO::Integer<16, true>;
using uint128_t = JIO::Integer<16, false>;

int main(int, char**) {

    int128_t a = 1;
    uint128_t b = 2;

    std::cout << (a | b) << std::endl; // 3

    return 0;
}
```

```c++
#include <iostream>
#include "Integer.hpp"

using namespace JIO;

int main(int, char**) {

    // Integer<1, false> (equivalent to uint8_t)
    auto a = 1_UI;

    // Integer<2, true> (equivalent to int16_t)
    auto b = 0xfff_SI;

    // Integer<2, true> (equivalent to int16_t)
    auto r = a + b;

    std::cout << std::hex << std::showbase;
    std::cout << r << std::endl; // 0x1000

    return 0;
}
```
## Compilation
A C++ compiler supporting at least C++14 is required.

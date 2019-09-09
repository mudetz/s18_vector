# s18_vector
[![Build Status](https://travis-ci.com/mudetz/s18_vector.svg?token=Zzkozptv3erZZycPNKT4&branch=master)](https://travis-ci.com/mudetz/s18_vector_definitive)

Add-on for [sdsl-lite](https://github.com/simongog/sdsl-lite) which implements the `s18_vector`
compressed bit vector.  
Supports `access`, `rank<1>`, `select<1>`, as proposed by [Arroyuelo (et al)][1].

## Usage

Install [sdsl-lite](https://github.com/simongog/sdsl-lite).  
Then include the `s18_vector` header from `include/sdsl/s18_vector.hpp`.

For example:
```cpp
#include <iostream>
#include <sdsl/int_vector.hpp>
#include <sdsl/rrr_vector.hpp>
#include <sdsl/sd_vector.hpp>
#include "s18_vector.hpp"

int main(void)
{
    sdsl::bit_vector b = sdsl::bit_vector(80 * (1 << 20), 0);
    for (size_t i = 0; i < b.size(); i += 100)
        b[i] = 1;

    std::cout << "Size in bytes" << std::endl;
    std::cout << "\toriginal:\t" << sdsl::size_in_bytes(b) << std::endl;

    sdsl::rrr_vector<63> rrrb(b);
    std::cout << "\trrr\t\t" << sdsl::size_in_bytes(rrrb) << std::endl;

    sdsl::sd_vector<> sdb(b);
    std::cout << "\tsd\t\t" << sdsl::size_in_bytes(sdb) << std::endl;

    sdsl::s18_vector<> s18b(b);
    sdt::cout << "\ts18\t\t" << sdsl::size_in_bytes(s18b) << std::endl;

    return 0;
}
```

[1]: Arroyuelo, D., Oyarzún, M., González, S., & Sepulveda, V. (2018). Hybrid compression of inverted
lists for reordered document collections. Information Processing & Management, 54(6), 1308-1324.

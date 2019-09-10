/*
 * s18_vector: An implementation for S18 compressed bitvectors
 * Copyright (C) 2019  Manuel Weitzman

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <sdsl/int_vector.hpp>
#include <sdsl/rrr_vector.hpp>
#include <sdsl/sd_vector.hpp>
#include "s18_vector.hpp"

#define SIZE 10000
#define SIZE_PLOT 30


int main(void)
{
	sdsl::bit_vector b = sdsl::bit_vector(SIZE, 0);

	std::random_device g;
	std::geometric_distribution<uint32_t> d(.6);

	size_t i = d(g) + 1;
	while (i < b.size()) {
		b[i] = 1;
		i += d(g) + 1;
	}

	sdsl::rrr_vector<63>  rrrb(b);
	sdsl::sd_vector<>     sdb(b);
	sdsl::s18_vector<64>  s1864(bv);

	return 0;
}

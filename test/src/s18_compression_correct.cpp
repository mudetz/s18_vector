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
#ifndef TEST_S18
#define TEST_S18
#endif

#include <algorithm>
#include <numeric>
#include <random>
#include <string>
#include <sdsl/int_vector.hpp>
#include "s18_vector.hpp"
#include "catch.hpp"


sdsl::int_vector<> gap_vector_gen(size_t, size_t, bool);
sdsl::bit_vector gap_to_bv(sdsl::int_vector<>);
size_t auto_shift(uint32_t, size_t, size_t, size_t);

static size_t const case_packed_numbers[18] = {28,14, 9, 7, 4, 3, 2,28,14, 9, 7, 4, 3, 2, 5, 1, 5, 1};
static size_t const case_number_chunks[18]  = { 1, 2, 3, 4, 7, 9,14, 1, 2, 3, 4, 7, 9,14, 5,28, 5, 1};
static bool   const case_preceding_ones[18] = { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0};
static uint32_t const cases[18] = {
	0x00000000,  // C1
	0x10000000,  // C2
	0x20000000,  // C3
	0x30000000,  // C4
	0x40000000,  // C5
	0x50000000,  // C6
	0x60000000,  // C7
	0x70000000,  // C8
	0x80000000,  // C9
	0x90000000,  // C10
	0xA0000000,  // C11
	0xB0000000,  // C12
	0xC0000000,  // C13
	0xD0000000,  // C14
	0xE0000000,  // C15
	0xF8000000,  // C16
	0xF0000000,  // C17
	0xF4000000   // C18
};

static std::vector<size_t> const block_sizes = {8,16,32,64,128,256,512};

TEMPLATE_TEST_CASE_SIG("All cases are compressed correctly", "[compression]", ((uint16_t B), B), (64), (128), (256), (512), (1024))
{
	for (size_t C = 0; C < 17; C++)
		DYNAMIC_SECTION("Compression is correct for C" << std::to_string(C + 1))
		{
			size_t const bits = case_packed_numbers[C];
			size_t const chunks  = case_number_chunks[C];
			bool   const preceding_ones = case_preceding_ones[C];

			sdsl::int_vector<> gv = gap_vector_gen(bits, chunks, preceding_ones);
			sdsl::int_vector<> av(gv.size(), 0); std::partial_sum(gv.begin(), gv.end(), av.begin());
			sdsl::bit_vector bv = gap_to_bv(gv);

			WHEN("It is compressed")
			{
				sdsl::s18_vector<B> s18(bv);
				THEN ("It is correctly encoded in a S18 word")
				{

					/* Check encoding size */
					REQUIRE(s18.debug__s18_seq().size() == 1);

					/* Check encoding header */
					if (0 <= C and C <= 14) { /* Cases 1-15 */
						REQUIRE((s18.debug__s18_seq()[0] & 0xF0000000) == cases[C]);
					} else if (C == 15) {/* Case 16 */
						REQUIRE((s18.debug__s18_seq()[0] & 0xF8000000) == cases[C]);
					} else if (C == 16) {/* Case 17 */
						REQUIRE((s18.debug__s18_seq()[0] & 0xF0000000) == cases[C]);
					}

					/* Check encoding body */
					if (0 <= C and C <= 6) { /* Cases 1-7 */
						for (size_t i = 0; i < chunks; i++)
							REQUIRE(auto_shift(s18.debug__s18_seq()[0], bits, i, chunks) == gv[i]);
					} else if (7 <= C and C <= 14) {/* Cases 8-15 */
						for (size_t i = 0; i < chunks; i++)
							REQUIRE(auto_shift(s18.debug__s18_seq()[0], bits, i, chunks) == gv[i + 28]);
					} else if (C == 15) { /* Case 16 */
						REQUIRE((s18.debug__s18_seq()[0] & ~0xF8000000) == gv.size());
					} else if (C == 16) {/* Case 17 */
						for (size_t i = 0; i < chunks; i++)
							REQUIRE(auto_shift(s18.debug__s18_seq()[0], bits, i, chunks) == gv[i]);
					}
				}
				AND_THEN("Size is correct")
				{
					REQUIRE(bv.size() == s18.size());
				}
#if 0
				AND_THEN("It is decompressed correctly (using access)")
				{
					for (size_t i = 0; i < bv.size(); i++)
						REQUIRE(bv[i] == s18[i]);
				}
				AND_THEN("It is decompressed correctly (using rank)")
				{
					for (size_t i = 0; i < bv.size(); i++)
						REQUIRE(rs(i) == rv[i]);
				}
				AND_THEN("It is decompressed correctly (using select)")
				{
					for (size_t i = 0; i < av.size(); i++)
						REQUIRE(ss(i) == av[i]);
				}
#endif
			}
		}
}


sdsl::int_vector<> gap_vector_gen(size_t bits, size_t total, bool prepend_1s)
{
	/* Avoid OOMS */
	if (bits == 28) bits = 15;

	/* Generate empty vector */
	size_t size = total + (prepend_1s ? 28 : 0);
	sdsl::int_vector<> gv(size, 0);

	/* Create random generator */
	std::default_random_engine generator;
	std::uniform_int_distribution<uint64_t> distribution(1 << (bits - 1), (1 << bits) - 1);

	/* Fill vector with random gaps */
	for (size_t i = 0; i < size; i++)
		gv[i] = distribution(generator);

	/* Fill vector with leading 1s */
	for (size_t i = 0; prepend_1s and i < 28; i++)
		gv[i] = 1;

	/* Deliver vector to callee */
	return gv;
}


sdsl::bit_vector gap_to_bv(sdsl::int_vector<> gv)
{
	/* Sum gv elements to get bit vector size */
	size_t size = std::accumulate(gv.begin(), gv.end(), (size_t) 0);

	/* Create bit vector */
	sdsl::bit_vector bv(size, 0);

	/* Fill with 1 by "jumping" each gv gap */
	int64_t it = -1;
	for (int64_t gap : gv) {
		it += gap;
		bv[it] = 1;
	}

	/* Deliver bit vector to callee */
	return bv;
}


size_t auto_shift(uint32_t word, size_t chunk_size, size_t chunk_position, size_t total_chunks)
{
	uint32_t mask = (1 << chunk_size) - 1;
	size_t bitshift = chunk_size * (total_chunks - 1 - chunk_position);

	word &= mask << bitshift;
	word >>= bitshift;
	return word;
}

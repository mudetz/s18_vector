/*
 * s18::vector: An implementation for S18 compressed bitvectors
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
#define RANDOM_ITERATIONS 100

#include <algorithm>
#include <numeric>
#include <random>
#include <string>
#include <sdsl/int_vector.hpp>
#include "s18_vector.hpp"
#include "catch.hpp"


sdsl::int_vector<64> gap_vector_gen(uint64_t, uint64_t, bool);
sdsl::bit_vector gap_to_bv(sdsl::int_vector<64>);
sdsl::int_vector<64> bv_to_rank(sdsl::bit_vector, uint64_t);
uint32_t auto_shift(uint32_t, uint64_t, uint64_t, uint64_t);

static uint64_t const case_packed_numbers[17] = {28,14, 9, 7, 4, 3, 2,28,14, 9, 7, 4, 3, 2, 5, 1, 5};
static uint64_t const case_number_chunks[17]  = { 1, 2, 3, 4, 7, 9,14, 1, 2, 3, 4, 7, 9,14, 5,28, 5};
static bool   const case_preceding_ones[17] = { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0};
static uint32_t const cases[17] = {
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
	0xF0000000,  // C16
	0xF8000000,  // C17
};


class Generator : public Catch::Generators::IGenerator<sdsl::bit_vector>
{
	private:
		sdsl::bit_vector bv;
		double x;
	public:
		Generator(uint64_t s, double _x)
			: bv(s, 0)
			, x(_x)
		{
			static_cast<void>(next());
		}

		sdsl::bit_vector const& get() const override
		{
			return bv;
		}
		bool next() override
		{
			for (uint64_t i = 0; i < bv.size(); i++)
				bv[i] = 0;

			std::random_device generator;
			std::geometric_distribution<uint64_t> distribution(x);

			uint64_t i = distribution(generator);
			while (i < bv.size()) {
				bv[i] = 1;
				i += distribution(generator) + 1;
			}

			return true;
		}
};


TEMPLATE_TEST_CASE_SIG("All cases are compressed correctly", "[compression]", ((uint16_t B), B), (64), (128), (256), (512), (1024))
{
	for (uint64_t C = 0; C < 17; C++)
		DYNAMIC_SECTION("Compression is correct for C" << std::to_string(C + 1))
		{
			uint64_t const bits = case_packed_numbers[C];
			uint64_t const chunks  = case_number_chunks[C];
			bool   const preceding_ones = case_preceding_ones[C];

			sdsl::int_vector<64> gv = gap_vector_gen(bits, chunks, preceding_ones);
			sdsl::int_vector<64> av(gv.size(), 0); std::partial_sum(gv.begin(), gv.end(), av.begin());
			sdsl::bit_vector bv = gap_to_bv(gv);
			sdsl::int_vector<64> rv1 = bv_to_rank(bv, 1);
			sdsl::int_vector<64> rv0 = bv_to_rank(bv, 0);

			WHEN("It is compressed")
			{
				sdsl::s18::vector<B> s18(bv);
				THEN ("It is correctly encoded in a S18 word")
				{

					/* Check encoding size */
					REQUIRE(s18.data().size() == 1);

					/* Check encoding header */
					if (0 <= C and C <= 14) { /* Cases 1-15 */
						REQUIRE((s18.data()[0] & 0xF0000000) == cases[C]);
					} else if (C == 15) {/* Case 16 */
						REQUIRE((s18.data()[0] & 0xF8000000) == cases[C]);
					} else if (C == 16) {/* Case 17 */
						REQUIRE((s18.data()[0] & 0xF8000000) == cases[C]);
					}

					/* Check encoding body */
					if (0 <= C and C <= 6) { /* Cases 1-7 */
						for (uint64_t i = 0; i < chunks; i++)
							REQUIRE(auto_shift(s18.data()[0], bits, i, chunks) == gv[i]);
					} else if (7 <= C and C <= 14) {/* Cases 8-15 */
						for (uint64_t i = 0; i < chunks; i++)
							REQUIRE(auto_shift(s18.data()[0], bits, i, chunks) == gv[i + 28]);
					} else if (C == 15) { /* Case 16 */
						REQUIRE((s18.data()[0] & ~0xF8000000) == gv.size());
					} else if (C == 16) {/* Case 17 */
						for (uint64_t i = 0; i < chunks; i++)
							REQUIRE(auto_shift(s18.data()[0], bits, i, chunks) == gv[i]);
					}
				}
				AND_THEN("Size is correct")
				{
					REQUIRE(bv.size() == s18.size());
				}
				AND_THEN("Word size is correct")
				{
					REQUIRE(sdsl::s18::word(s18.data()[0]).size() == gv.size());
				}
				AND_THEN ("Gaps are decoded correctly")
				{
					for (uint64_t i = 0; i < sdsl::s18::word(s18.data()[0]).size(); i++)
						REQUIRE(gv[i] == sdsl::s18::word(*s18.data().begin())[i]);
				}
				AND_THEN("It is decompressed correctly (using [slow] access)")
				{
					for (uint64_t i = 0; i < bv.size(); i++)
						REQUIRE(bv[i] == s18.slow_access(i));
				}
				AND_THEN("It is decompressed correctly (using [indexed] access)")
				{
					for (uint64_t i = 0; i < bv.size(); i++)
						REQUIRE(bv[i] == s18[i]);
				}
				AND_THEN("It is decompressed correctly (using access support)")
				{
					sdsl::s18::access_support<B> as(s18);
					for (uint64_t i = 0; i < bv.size(); i++)
						REQUIRE(bv[i] == as(i));
				}
				AND_THEN("It is decompressed correctly (using rank1)")
				{
					sdsl::s18::rank_support<1, B> rs(s18);
					for (uint64_t i = 0; i < bv.size(); i++)
						REQUIRE(rs(i) == rv1[i]);
				}
				AND_THEN("It is decompressed correctly (using rank0)")
				{
					sdsl::s18::rank_support<0, B> rs(s18);
					for (uint64_t i = 0; i < bv.size(); i++)
						REQUIRE(rs(i) == rv0[i]);
				}
				AND_THEN("It is decompressed correctly (using select1)")
				{
					sdsl::s18::select_support<1, B> ss(s18);
					for (uint64_t i = 0; i < av.size(); i++)
						REQUIRE(ss(i + 1) == av[i]);
				}
			}
		}
}


TEMPLATE_TEST_CASE_SIG("100 sparse vectors are compressed correctly", "", ((uint16_t B), B), (8), (16), (32), (64), (128), (256), (512), (1024))
{
	GIVEN("A bit vector")
	{
		sdsl::bit_vector bv = GENERATE(take(RANDOM_ITERATIONS, GeneratorWrapper<sdsl::bit_vector>(std::unique_ptr<IGenerator<sdsl::bit_vector>>(new Generator(2000, .1)))));
		sdsl::int_vector<64> av(sdsl::util::cnt_one_bits(bv), 0);

		for (uint64_t i = 0, j = 0; i < bv.size(); i++)
			if (bv[i]) av[j++] = i + 1;

		sdsl::int_vector<64> gv(sdsl::util::cnt_one_bits(bv), 0);
		for (uint64_t i = 1; i < gv.size(); i++)
			gv[i] = av[i] - av[i - 1];
		gv[0] = av[0] + 1;

		sdsl::int_vector<64> rv1 = bv_to_rank(bv, 1);
		sdsl::int_vector<64> rv0 = bv_to_rank(bv, 0);

		WHEN("It is compressed")
		{
			sdsl::s18::vector<B> s18(bv);
			THEN("Size is correct")
			{
				REQUIRE(bv.size() == s18.size());
			}
			AND_THEN("It is decompressed correctly (using [indexed] access)")
			{
				for (uint64_t i = 0; i < bv.size(); i++)
					REQUIRE(bv[i] == s18[i]);
			}
			AND_THEN("It is decompressed correctly (using access support)")
			{
				sdsl::s18::access_support<B> as(s18);
				for (uint64_t i = 0; i < bv.size(); i++)
					REQUIRE(bv[i] == as(i));
			}
			AND_THEN("It is decompressed correctly (using rank1)")
			{
				sdsl::s18::rank_support<1, B> rs(s18);
				for (uint64_t i = 0; i < bv.size(); i++)
					REQUIRE(rs(i) == rv1[i]);
			}

			AND_THEN("It is decompressed correctly (using rank0)")
			{
				sdsl::s18::rank_support<0, B> rs(s18);
				for (uint64_t i = 0; i < bv.size(); i++)
					REQUIRE(rs(i) == rv0[i]);
			}
			AND_THEN("It is decompressed correctly (using select1)")
			{
				sdsl::s18::select_support<1, B> ss(s18);
				for (uint64_t i = 0; i < av.size(); i++)
					REQUIRE(ss(i + 1) == av[i]);
			}
		}
	}
}

TEMPLATE_TEST_CASE_SIG("100 normal vectors are compressed correctly", "", ((uint16_t B), B), (8), (16), (32), (64), (128), (256), (512), (1024))
{
	GIVEN("A bit vector")
	{
		sdsl::bit_vector bv = GENERATE(take(RANDOM_ITERATIONS, GeneratorWrapper<sdsl::bit_vector>(std::unique_ptr<IGenerator<sdsl::bit_vector>>(new Generator(2000, .5)))));
		sdsl::int_vector<64> av(sdsl::util::cnt_one_bits(bv), 0);

		for (uint64_t i = 0, j = 0; i < bv.size(); i++)
			if (bv[i]) av[j++] = i + 1;

		sdsl::int_vector<64> gv(sdsl::util::cnt_one_bits(bv), 0);
		for (uint64_t i = 1; i < gv.size(); i++)
			gv[i] = av[i] - av[i - 1];
		gv[0] = av[0] + 1;

		sdsl::int_vector<64> rv1 = bv_to_rank(bv, 1);
		sdsl::int_vector<64> rv0 = bv_to_rank(bv, 0);

		WHEN("It is compressed")
		{
			sdsl::s18::vector<B> s18(bv);
			THEN("Size is correct")
			{
				REQUIRE(bv.size() == s18.size());
			}
			AND_THEN("It is decompressed correctly (using [indexed] access)")
			{
				for (uint64_t i = 0; i < bv.size(); i++)
					REQUIRE(bv[i] == s18[i]);
			}
			AND_THEN("It is decompressed correctly (using access support)")
			{
				sdsl::s18::access_support<B> as(s18);
				for (uint64_t i = 0; i < bv.size(); i++)
					REQUIRE(bv[i] == as(i));
			}
			AND_THEN("It is decompressed correctly (using rank1)")
			{
				sdsl::s18::rank_support<1, B> rs(s18);
				for (uint64_t i = 0; i < bv.size(); i++)
					REQUIRE(rs(i) == rv1[i]);
			}

			AND_THEN("It is decompressed correctly (using rank0)")
			{
				sdsl::s18::rank_support<0, B> rs(s18);
				for (uint64_t i = 0; i < bv.size(); i++)
					REQUIRE(rs(i) == rv0[i]);
			}
			AND_THEN("It is decompressed correctly (using select1)")
			{
				sdsl::s18::select_support<1, B> ss(s18);
				for (uint64_t i = 0; i < av.size(); i++)
					REQUIRE(ss(i + 1) == av[i]);
			}
		}
	}
}

TEMPLATE_TEST_CASE_SIG("100 dense vectors are compressed correctly", "", ((uint16_t B), B), (8), (16), (32), (64), (128), (256), (512), (1024))
{
	GIVEN("A bit vector")
	{
		sdsl::bit_vector bv = GENERATE(take(RANDOM_ITERATIONS, GeneratorWrapper<sdsl::bit_vector>(std::unique_ptr<IGenerator<sdsl::bit_vector>>(new Generator(2000, .9)))));
		sdsl::int_vector<64> av(sdsl::util::cnt_one_bits(bv), 0);

		for (uint64_t i = 0, j = 0; i < bv.size(); i++)
			if (bv[i]) av[j++] = i + 1;

		sdsl::int_vector<64> gv(sdsl::util::cnt_one_bits(bv), 0);
		for (uint64_t i = 1; i < gv.size(); i++)
			gv[i] = av[i] - av[i - 1];
		gv[0] = av[0] + 1;

		sdsl::int_vector<64> rv1 = bv_to_rank(bv, 1);
		sdsl::int_vector<64> rv0 = bv_to_rank(bv, 0);

		WHEN("It is compressed")
		{
			sdsl::s18::vector<B> s18(bv);
			THEN("Size is correct")
			{
				REQUIRE(bv.size() == s18.size());
			}
			AND_THEN("It is decompressed correctly (using [indexed] access)")
			{
				for (uint64_t i = 0; i < bv.size(); i++)
					REQUIRE(bv[i] == s18[i]);
			}
			AND_THEN("It is decompressed correctly (using access support)")
			{
				sdsl::s18::access_support<B> as(s18);
				for (uint64_t i = 0; i < bv.size(); i++)
					REQUIRE(bv[i] == as(i));
			}
			AND_THEN("It is decompressed correctly (using rank1)")
			{
				sdsl::s18::rank_support<1, B> rs(s18);
				for (uint64_t i = 0; i < bv.size(); i++)
					REQUIRE(rs(i) == rv1[i]);
			}

			AND_THEN("It is decompressed correctly (using rank0)")
			{
				sdsl::s18::rank_support<0, B> rs(s18);
				for (uint64_t i = 0; i < bv.size(); i++)
					REQUIRE(rs(i) == rv0[i]);
			}
			AND_THEN("It is decompressed correctly (using select1)")
			{
				sdsl::s18::select_support<1, B> ss(s18);
				for (uint64_t i = 0; i < av.size(); i++)
					REQUIRE(ss(i + 1) == av[i]);
			}
		}
	}
}


sdsl::int_vector<64> gap_vector_gen(uint64_t bits, uint64_t total, bool prepend_1s)
{
	/* Avoid OOMS */
	if (bits == 28) bits = 15;

	/* Generate empty vector */
	uint64_t size = total + (prepend_1s ? 28 : 0);
	sdsl::int_vector<64> gv(size, 0);

	/* Create random generator */
	std::default_random_engine generator;
	std::uniform_int_distribution<uint64_t> distribution(1 << (bits - 1), (1 << bits) - 1);

	/* Fill vector with random gaps */
	for (uint64_t i = 0; i < size; i++)
		gv[i] = distribution(generator);

	/* Fill vector with leading 1s */
	for (uint64_t i = 0; prepend_1s and i < 28; i++)
		gv[i] = 1;

	/* Deliver vector to callee */
	return gv;
}


sdsl::bit_vector gap_to_bv(sdsl::int_vector<64> gv)
{
	/* Sum gv elements to get bit vector size */
	uint64_t size = std::accumulate(gv.begin(), gv.end(), (uint64_t) 0);

	/* Create bit vector */
	sdsl::bit_vector bv(size, 0);

	/* Fill with 1 by "jumping" each gv gap */
	uint64_t it = -1;
	for (auto &gap : gv) {
		it += gap;
		bv[it] = 1;
	}

	/* Deliver bit vector to callee */
	return bv;
}


uint32_t auto_shift(uint32_t word, uint64_t chunk_size, uint64_t chunk_position, uint64_t total_chunks)
{
	uint32_t mask = (1 << chunk_size) - 1;
	uint64_t bitshift = chunk_size * (total_chunks - 1 - chunk_position);

	word &= mask << bitshift;
	word >>= bitshift;
	return word;
}

sdsl::int_vector<64> bv_to_rank(sdsl::bit_vector bv, uint64_t bit)
{
	sdsl::int_vector<64> ranks(bv.size(), 0);

	uint64_t seen_ones = 0;
	for (uint64_t i = 0; i < bv.size(); i++) {
		ranks[i] = seen_ones;
		if (bv[i] == bit) seen_ones++;
	}

	return ranks;
}

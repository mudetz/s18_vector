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
#include <sdsl/hyb_vector.hpp>
#include "s18_vector.hpp"
#include "s9_vector.hpp"

#define SIZE 100000000
#define RUNS 1000

int main(void)
{
	double constexpr T[] = {.95, .5, .05};

	for (size_t t = 0; t < 3; t++) {
		std::random_device g;
    		std::uniform_real_distribution<double> unif(0, 1);
    		std::uniform_int_distribution<size_t> unifint(SIZE / 2000, SIZE / 1000);
    		std::uniform_int_distribution<size_t> unifidx(0, SIZE - SIZE / 1000);

		sdsl::bit_vector b = sdsl::bit_vector(SIZE, 0);
		sdsl::rank_support_v rs(&b);
		sdsl::select_support_mcl ss(&b);

		// Gaps
	 	for (size_t i = 0; i < b.size(); i++)
	 		b[i] = unif(g) < T[t];

		// Runs
		for (size_t i = 0; i < RUNS; i++) {
			size_t run_size = unifint(g);
			size_t start = unifidx(g);
			for (size_t j = 0; j < run_size; j++)
				b[start + j] = 1;
		}


		sdsl::s9_vector<8> s9_8(b);
		sdsl::s9_vector<16> s9_16(b);
		sdsl::s9_vector<32> s9_32(b);
		sdsl::s9_vector<64> s9_64(b);
		sdsl::s9_vector<128> s9_128(b);
		sdsl::s9_vector<256> s9_256(b);
		sdsl::s9_vector<512> s9_512(b);

		sdsl::s18::vector<1> s18_1(b);
		sdsl::s18::vector<2> s18_2(b);
		sdsl::s18::vector<4> s18_4(b);
		sdsl::s18::vector<8> s18_8(b);
		sdsl::s18::vector<16> s18_16(b);
		sdsl::s18::vector<32> s18_32(b);
		sdsl::s18::vector<64> s18_64(b);

		sdsl::rrr_vector<8>   rrr_8(b);
		sdsl::rrr_vector<16>  rrr_16(b);
		sdsl::rrr_vector<32>  rrr_32(b);
		sdsl::rrr_vector<64>  rrr_64(b);
		sdsl::rrr_vector<128> rrr_128(b);
		sdsl::rrr_vector<256> rrr_256(b);

		sdsl::rank_support_rrr<1, 8>   rs_rrr_8(&rrr_8);
		sdsl::rank_support_rrr<1, 16>  rs_rrr_16(&rrr_16);
		sdsl::rank_support_rrr<1, 32>  rs_rrr_32(&rrr_32);
		sdsl::rank_support_rrr<1, 64>  rs_rrr_64(&rrr_64);
		sdsl::rank_support_rrr<1, 128> rs_rrr_128(&rrr_128);
		sdsl::rank_support_rrr<1, 256> rs_rrr_256(&rrr_256);

		sdsl::select_support_rrr<1, 8>   ss_rrr_8(&rrr_8);
		sdsl::select_support_rrr<1, 16>  ss_rrr_16(&rrr_16);
		sdsl::select_support_rrr<1, 32>  ss_rrr_32(&rrr_32);
		sdsl::select_support_rrr<1, 64>  ss_rrr_64(&rrr_64);
		sdsl::select_support_rrr<1, 128> ss_rrr_128(&rrr_128);
		sdsl::select_support_rrr<1, 256> ss_rrr_256(&rrr_256);

		sdsl::sd_vector sd_1(b);

		sdsl::rank_support_sd<1> rs_sd_1(&sd_1);
		sdsl::select_support_sd<1> ss_sd_1(&sd_1);

		sdsl::hyb_vector hv(b);
		sdsl::rank_support_hyb rs_hyb(&hv);

		std::cout << "CASE " << t << std::endl;

		std::cout << "bit_vector"      << "\t" << size_in_mega_bytes(b)       << "\t" << size_in_mega_bytes(rs) << "\t" << size_in_mega_bytes(ss) << std::endl;
		std::cout << "s9_vector<8>"    << "\t" << size_in_mega_bytes(s9_8)    << std::endl;
		std::cout << "s9_vector<16>"   << "\t" << size_in_mega_bytes(s9_16)   << std::endl;
		std::cout << "s9_vector<32>"   << "\t" << size_in_mega_bytes(s9_32)   << std::endl;
		std::cout << "s9_vector<64>"   << "\t" << size_in_mega_bytes(s9_64)   << std::endl;
		std::cout << "s9_vector<128>"  << "\t" << size_in_mega_bytes(s9_128)  << std::endl;
		std::cout << "s9_vector<256>"  << "\t" << size_in_mega_bytes(s9_256)  << std::endl;
		std::cout << "s9_vector<512>"  << "\t" << size_in_mega_bytes(s9_512)  << std::endl;
		std::cout << "s18::vector<1>"  << "\t" << size_in_mega_bytes(s18_1)   << std::endl;
		std::cout << "s18::vector<2>"  << "\t" << size_in_mega_bytes(s18_2)   << std::endl;
		std::cout << "s18::vector<4>"  << "\t" << size_in_mega_bytes(s18_4)   << std::endl;
		std::cout << "s18::vector<8>"  << "\t" << size_in_mega_bytes(s18_8)   << std::endl;
		std::cout << "s18::vector<16>" << "\t" << size_in_mega_bytes(s18_16)  << std::endl;
		std::cout << "s18::vector<32>" << "\t" << size_in_mega_bytes(s18_32)  << std::endl;
		std::cout << "s18::vector<64>" << "\t" << size_in_mega_bytes(s18_64)  << std::endl;
		std::cout << "rrr_vector<8>"   << "\t" << size_in_mega_bytes(rrr_8)   << "\t" << size_in_mega_bytes(  rs_rrr_8) << "\t" << size_in_mega_bytes(  ss_rrr_8) << std::endl;
		std::cout << "rrr_vector<16>"  << "\t" << size_in_mega_bytes(rrr_16)  << "\t" << size_in_mega_bytes( rs_rrr_16) << "\t" << size_in_mega_bytes( ss_rrr_16) << std::endl;
		std::cout << "rrr_vector<32>"  << "\t" << size_in_mega_bytes(rrr_32)  << "\t" << size_in_mega_bytes( rs_rrr_32) << "\t" << size_in_mega_bytes( ss_rrr_32) << std::endl;
		std::cout << "rrr_vector<64>"  << "\t" << size_in_mega_bytes(rrr_64)  << "\t" << size_in_mega_bytes( rs_rrr_64) << "\t" << size_in_mega_bytes( ss_rrr_64) << std::endl;
		std::cout << "rrr_vector<128>" << "\t" << size_in_mega_bytes(rrr_128) << "\t" << size_in_mega_bytes(rs_rrr_128) << "\t" << size_in_mega_bytes(ss_rrr_128) << std::endl;
		std::cout << "rrr_vector<256>" << "\t" << size_in_mega_bytes(rrr_256) << "\t" << size_in_mega_bytes(rs_rrr_256) << "\t" << size_in_mega_bytes(ss_rrr_256) << std::endl;
		std::cout << "sd_vector<>"     << "\t" << size_in_mega_bytes(sd_1) << "\t" << size_in_mega_bytes(rs_sd_1) << "\t" << size_in_mega_bytes(ss_sd_1) << std::endl;
		std::cout << "hyb_vector"      << "\t" << size_in_mega_bytes(hv) << "\t" << size_in_mega_bytes(rs_hyb) << std::endl;

		std::cout << std::endl;
	}

	return 0;
}

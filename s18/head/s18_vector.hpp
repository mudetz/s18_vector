/*
 * vector: An implementation for S18 compressed bitvectors
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
#ifndef INCLUDED_SDSL_S18_VECTOR
#define INCLUDED_SDSL_S18_VECTOR

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>

#include <sdsl/int_vector.hpp>
#include <sdsl/vlc_vector.hpp>
#include <sdsl/util.hpp>

#include "constants.hpp"


namespace sdsl
{
namespace s18
{

/*
 * Forward declarations
 */

/* Access */
template<uint16_t b_s = 256, class vector_type = int_vector<32>>
class access_support;

/* Rank */
template<uint8_t q = 1, uint16_t b_s = 256, class vector_type = int_vector<32>>
class rank_support;

/* Select */
template<uint8_t q = 1, uint16_t b_s = 256, class vector_type = int_vector<32>>
class select_support;

/* S18 word */
class word;

/* S18 vector */
template<uint16_t b_s = 256, class vector_type = int_vector<32>>
class vector;


/*
 * S18 word
 */
class word
{
	private:
		bool                  already_packed;
		bool                  processing_lead_1s;
		uint64_t              leading_1s;
		std::vector<uint32_t> pending_gaps;

		static uint64_t const BIT_PAD[33];
		static uint64_t const BITS_TO_CHUNKS[29];
		static uint64_t const DECODER_CHUNKS[17];
		static uint64_t const DECODER_BITS[17];
		static uint32_t const DECODER_MASK[17][14];
		static uint32_t const DECODER_SHIFT[17][14];
	public:
		uint32_t value;
		uint64_t chunk_size;
		word(void)
			: already_packed(false)
			, processing_lead_1s(true)
			, leading_1s(0)
			, pending_gaps()
			, value(0)
			, chunk_size(1)
		{}

		word(uint32_t const w)
			: already_packed(true)
			, processing_lead_1s(false)
			, leading_1s(0)
			, pending_gaps()
			, value(w)
			, chunk_size(0)
		{}

	public:
		std::tuple<uint64_t, uint64_t, uint64_t> metadata(void) const
		{
			// Return leading 1s, case as int and len w/o leading ones
			switch (value & MASK_HEADER4) {
				case CASE01: return std::make_tuple(0,  0, CHUNKS_CASE01);
				case CASE02: return std::make_tuple(1,  0, CHUNKS_CASE02);
				case CASE03: return std::make_tuple(2,  0, CHUNKS_CASE03);
				case CASE04: return std::make_tuple(3,  0, CHUNKS_CASE04);
				case CASE05: return std::make_tuple(4,  0, CHUNKS_CASE05);
				case CASE06: return std::make_tuple(5,  0, CHUNKS_CASE06);
				case CASE07: return std::make_tuple(6,  0, CHUNKS_CASE07);
				case CASE08: return std::make_tuple(0, 28, CHUNKS_CASE01);
				case CASE09: return std::make_tuple(1, 28, CHUNKS_CASE02);
				case CASE10: return std::make_tuple(2, 28, CHUNKS_CASE03);
				case CASE11: return std::make_tuple(3, 28, CHUNKS_CASE04);
				case CASE12: return std::make_tuple(4, 28, CHUNKS_CASE05);
				case CASE13: return std::make_tuple(5, 28, CHUNKS_CASE06);
				case CASE14: return std::make_tuple(6, 28, CHUNKS_CASE07);
				case CASE15: return std::make_tuple(16, 28, CHUNKS_CASE17);
				default: switch (value & MASK_HEADER5) {
					case CASE16: return std::make_tuple(15, (value & MASK_BODY5), 0);
					case CASE17: return std::make_tuple(16, 0, CHUNKS_CASE17);
					default: throw std::invalid_argument("word::metadata: Invalid case");
				}
			}
		}

		uint64_t access_fast(uint64_t key, uint64_t _case) const
		{
			assert(_case != 15);
			return (value & DECODER_MASK[_case][key]) >> DECODER_SHIFT[_case][key];
		}

		bool add_if_enough_space(uint32_t gap)
		{
			assert(not already_packed);
			processing_lead_1s = processing_lead_1s and gap == 1;

			if (processing_lead_1s) {
				if (leading_1s < 0x07FFFFFF) leading_1s += 1;
				return leading_1s < 0x07FFFFFF;
			}

			if (leading_1s < 28) while (leading_1s) {
				pending_gaps.push_back(1);
				leading_1s--;
			}
			else if (leading_1s > 28) return false;

			uint64_t new_pending_size = pending_gaps.size() + 1;
			uint64_t gap_size = BIT_PAD[bits::hi(gap) + 1];
			uint64_t new_chunk_size = std::max(gap_size, chunk_size);
			if (new_pending_size * new_chunk_size > 28)
				return false;

			pending_gaps.push_back(gap);
			chunk_size = std::max(gap_size, chunk_size);
			return true;
		}

		uint32_t pack(void)
		{
			assert(not already_packed);
			already_packed = true;

			/* Handler for C16 */
			if (chunk_size == 1 and leading_1s)
				return (value = CASE16 | static_cast<uint32_t>(leading_1s));
			if (chunk_size == 1 and pending_gaps.size())
				return (value = CASE16 | static_cast<uint32_t>(pending_gaps.size()));

			for (uint32_t gap : pending_gaps) {
				value <<= chunk_size;
				value |= gap;
			}
			value <<= chunk_size * (BITS_TO_CHUNKS[chunk_size] - pending_gaps.size());
			pending_gaps.clear();

			switch (chunk_size) {
				case 28: value |= !leading_1s ? CASE01 : CASE08; break;
				case 14: value |= !leading_1s ? CASE02 : CASE09; break;
				case  9: value |= !leading_1s ? CASE03 : CASE10; break;
				case  7: value |= !leading_1s ? CASE04 : CASE11; break;
				case  4: value |= !leading_1s ? CASE05 : CASE12; break;
				case  3: value |= !leading_1s ? CASE06 : CASE13; break;
				case  2: value |= !leading_1s ? CASE07 : CASE14; break;
				case  5: value |= !leading_1s ? CASE17 : CASE15; break;
				default: throw std::invalid_argument("word::pack: Invalid case");
			}

			return value;
		}

#if DEBUG
		uint64_t size(void) const {
			switch (value & MASK_HEADER4) {
				case CASE01: return CHUNKS_CASE01;
				case CASE02: return CHUNKS_CASE02;
				case CASE03: return CHUNKS_CASE03;
				case CASE04: return CHUNKS_CASE04;
				case CASE05: return CHUNKS_CASE05;
				case CASE06: return CHUNKS_CASE06;
				case CASE07: return CHUNKS_CASE07;
				case CASE08: return CHUNKS_CASE08 + 28;
				case CASE09: return CHUNKS_CASE09 + 28;
				case CASE10: return CHUNKS_CASE10 + 28;
				case CASE11: return CHUNKS_CASE11 + 28;
				case CASE12: return CHUNKS_CASE12 + 28;
				case CASE13: return CHUNKS_CASE13 + 28;
				case CASE14: return CHUNKS_CASE14 + 28;
				case CASE15: return CHUNKS_CASE15 + 28;
				default: switch (value & MASK_HEADER5) {
					case CASE16: return value & MASK_BODY5;
					case CASE17: return CHUNKS_CASE17;
					default: throw std::invalid_argument("word::size: Invalid case");
				}
			}
		}

		uint64_t operator[](uint64_t key) const
		{
			switch (value & MASK_HEADER4) {
				case CASE01: return (value & (MASK_CASE01_CHUNK >> (key * BITS_CASE01))) >> (BITS_CASE01 * (CHUNKS_CASE01 - key - 1));
				case CASE02: return (value & (MASK_CASE02_CHUNK >> (key * BITS_CASE02))) >> (BITS_CASE02 * (CHUNKS_CASE02 - key - 1));
				case CASE03: return (value & (MASK_CASE03_CHUNK >> (key * BITS_CASE03))) >> (BITS_CASE03 * (CHUNKS_CASE03 - key - 1));
				case CASE04: return (value & (MASK_CASE04_CHUNK >> (key * BITS_CASE04))) >> (BITS_CASE04 * (CHUNKS_CASE04 - key - 1));
				case CASE05: return (value & (MASK_CASE05_CHUNK >> (key * BITS_CASE05))) >> (BITS_CASE05 * (CHUNKS_CASE05 - key - 1));
				case CASE06: return (value & (MASK_CASE06_CHUNK >> (key * BITS_CASE06))) >> (BITS_CASE06 * (CHUNKS_CASE06 - key - 1));
				case CASE07: return (value & (MASK_CASE07_CHUNK >> (key * BITS_CASE07))) >> (BITS_CASE07 * (CHUNKS_CASE07 - key - 1));
				case CASE08: return key < 28 ? 1 : (value & (MASK_CASE08_CHUNK >> ((key - 28) * BITS_CASE08))) >> (BITS_CASE08 * (CHUNKS_CASE08 - (key - 28) - 1));
				case CASE09: return key < 28 ? 1 : (value & (MASK_CASE09_CHUNK >> ((key - 28) * BITS_CASE09))) >> (BITS_CASE09 * (CHUNKS_CASE09 - (key - 28) - 1));
				case CASE10: return key < 28 ? 1 : (value & (MASK_CASE10_CHUNK >> ((key - 28) * BITS_CASE10))) >> (BITS_CASE10 * (CHUNKS_CASE10 - (key - 28) - 1));
				case CASE11: return key < 28 ? 1 : (value & (MASK_CASE11_CHUNK >> ((key - 28) * BITS_CASE11))) >> (BITS_CASE11 * (CHUNKS_CASE11 - (key - 28) - 1));
				case CASE12: return key < 28 ? 1 : (value & (MASK_CASE12_CHUNK >> ((key - 28) * BITS_CASE12))) >> (BITS_CASE12 * (CHUNKS_CASE12 - (key - 28) - 1));
				case CASE13: return key < 28 ? 1 : (value & (MASK_CASE13_CHUNK >> ((key - 28) * BITS_CASE13))) >> (BITS_CASE13 * (CHUNKS_CASE13 - (key - 28) - 1));
				case CASE14: return key < 28 ? 1 : (value & (MASK_CASE14_CHUNK >> ((key - 28) * BITS_CASE14))) >> (BITS_CASE14 * (CHUNKS_CASE14 - (key - 28) - 1));
				case CASE15: return key < 28 ? 1 : (value & (MASK_CASE15_CHUNK >> ((key - 28) * BITS_CASE15))) >> (BITS_CASE15 * (CHUNKS_CASE15 - (key - 28) - 1));
				default: switch (value & MASK_HEADER5) {
					case CASE16: return key < (value & MASK_CASE16_CHUNK);
					case CASE17: return (value & (MASK_CASE17_CHUNK >> (key * BITS_CASE17))) >> (BITS_CASE17 * (CHUNKS_CASE17 - key - 1));
					default: throw std::invalid_argument("word::operator[]: Invalid case");
				}
			}
		}
#endif
};

uint64_t const word::BIT_PAD[33] = { 1, 1, 2, 3, 4, 5, 7, 7, 9, 9, 14, 14, 14, 14, 14, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28 };
uint64_t const word::BITS_TO_CHUNKS[29] = { 0,28,14,9,7,5,0,4,0,3,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
uint64_t const word::DECODER_CHUNKS[17] = {1,2,3,4,7,9,14,1,2,3,4,7,9,14,5,0,5};
uint64_t const word::DECODER_BITS[17] = {28,14,9,7,4,3,2,28,14,9,7,4,3,2,5,0,5};
uint32_t const word::DECODER_MASK[17][14] = {
	{MASK_CASE01_CHUNK >> (0 * BITS_CASE01)},
	{MASK_CASE02_CHUNK >> (0 * BITS_CASE02), MASK_CASE02_CHUNK >> (1 * BITS_CASE02)},
	{MASK_CASE03_CHUNK >> (0 * BITS_CASE03), MASK_CASE03_CHUNK >> (1 * BITS_CASE03), MASK_CASE03_CHUNK >> (2 * BITS_CASE03)},
	{MASK_CASE04_CHUNK >> (0 * BITS_CASE04), MASK_CASE04_CHUNK >> (1 * BITS_CASE04), MASK_CASE04_CHUNK >> (2 * BITS_CASE04), MASK_CASE04_CHUNK >> (3 * BITS_CASE04)},
	{MASK_CASE05_CHUNK >> (0 * BITS_CASE05), MASK_CASE05_CHUNK >> (1 * BITS_CASE05), MASK_CASE05_CHUNK >> (2 * BITS_CASE05), MASK_CASE05_CHUNK >> (3 * BITS_CASE05), MASK_CASE05_CHUNK >> (4 * BITS_CASE05), MASK_CASE05_CHUNK >> (5 * BITS_CASE05), MASK_CASE05_CHUNK >> (6 * BITS_CASE05)},
	{MASK_CASE06_CHUNK >> (0 * BITS_CASE06), MASK_CASE06_CHUNK >> (1 * BITS_CASE06), MASK_CASE06_CHUNK >> (2 * BITS_CASE06), MASK_CASE06_CHUNK >> (3 * BITS_CASE06), MASK_CASE06_CHUNK >> (4 * BITS_CASE06), MASK_CASE06_CHUNK >> (5 * BITS_CASE06), MASK_CASE06_CHUNK >> (6 * BITS_CASE06), MASK_CASE06_CHUNK >> (7 * BITS_CASE06), MASK_CASE06_CHUNK >> (8 * BITS_CASE06)},
	{MASK_CASE07_CHUNK >> ( 0 * BITS_CASE07), MASK_CASE07_CHUNK >> ( 1 * BITS_CASE07), MASK_CASE07_CHUNK >> ( 2 * BITS_CASE07), MASK_CASE07_CHUNK >> ( 3 * BITS_CASE07), MASK_CASE07_CHUNK >> ( 4 * BITS_CASE07), MASK_CASE07_CHUNK >> ( 5 * BITS_CASE07), MASK_CASE07_CHUNK >> ( 6 * BITS_CASE07), MASK_CASE07_CHUNK >> ( 7 * BITS_CASE07), MASK_CASE07_CHUNK >> ( 8 * BITS_CASE07), MASK_CASE07_CHUNK >> ( 9 * BITS_CASE07), MASK_CASE07_CHUNK >> (10 * BITS_CASE07), MASK_CASE07_CHUNK >> (11 * BITS_CASE07), MASK_CASE07_CHUNK >> (12 * BITS_CASE07), MASK_CASE07_CHUNK >> (13 * BITS_CASE07)},
	{MASK_CASE08_CHUNK >> (0 * BITS_CASE08)},
	{MASK_CASE09_CHUNK >> (0 * BITS_CASE09), MASK_CASE09_CHUNK >> (1 * BITS_CASE09)},
	{MASK_CASE10_CHUNK >> (0 * BITS_CASE10), MASK_CASE10_CHUNK >> (1 * BITS_CASE10), MASK_CASE10_CHUNK >> (2 * BITS_CASE10)},
	{MASK_CASE11_CHUNK >> (0 * BITS_CASE11), MASK_CASE11_CHUNK >> (1 * BITS_CASE11), MASK_CASE11_CHUNK >> (2 * BITS_CASE11), MASK_CASE11_CHUNK >> (3 * BITS_CASE11)},
	{MASK_CASE12_CHUNK >> (0 * BITS_CASE12), MASK_CASE12_CHUNK >> (1 * BITS_CASE12), MASK_CASE12_CHUNK >> (2 * BITS_CASE12), MASK_CASE12_CHUNK >> (3 * BITS_CASE12), MASK_CASE12_CHUNK >> (4 * BITS_CASE12), MASK_CASE12_CHUNK >> (5 * BITS_CASE12), MASK_CASE12_CHUNK >> (6 * BITS_CASE12)},
	{MASK_CASE13_CHUNK >> (0 * BITS_CASE13), MASK_CASE13_CHUNK >> (1 * BITS_CASE13), MASK_CASE13_CHUNK >> (2 * BITS_CASE13), MASK_CASE13_CHUNK >> (3 * BITS_CASE13), MASK_CASE13_CHUNK >> (4 * BITS_CASE13), MASK_CASE13_CHUNK >> (5 * BITS_CASE13), MASK_CASE13_CHUNK >> (6 * BITS_CASE13), MASK_CASE13_CHUNK >> (7 * BITS_CASE13), MASK_CASE13_CHUNK >> (8 * BITS_CASE13)},
	{MASK_CASE14_CHUNK >> ( 0 * BITS_CASE14), MASK_CASE14_CHUNK >> ( 1 * BITS_CASE14), MASK_CASE14_CHUNK >> ( 2 * BITS_CASE14), MASK_CASE14_CHUNK >> ( 3 * BITS_CASE14), MASK_CASE14_CHUNK >> ( 4 * BITS_CASE14), MASK_CASE14_CHUNK >> ( 5 * BITS_CASE14), MASK_CASE14_CHUNK >> ( 6 * BITS_CASE14), MASK_CASE14_CHUNK >> ( 7 * BITS_CASE14), MASK_CASE14_CHUNK >> ( 8 * BITS_CASE14), MASK_CASE14_CHUNK >> ( 9 * BITS_CASE14), MASK_CASE14_CHUNK >> (10 * BITS_CASE14), MASK_CASE14_CHUNK >> (11 * BITS_CASE14), MASK_CASE14_CHUNK >> (12 * BITS_CASE14), MASK_CASE14_CHUNK >> (13 * BITS_CASE14)},
	{MASK_CASE15_CHUNK >> (0 * BITS_CASE15), MASK_CASE15_CHUNK >> (1 * BITS_CASE15), MASK_CASE15_CHUNK >> (2 * BITS_CASE15), MASK_CASE15_CHUNK >> (3 * BITS_CASE15), MASK_CASE15_CHUNK >> (4 * BITS_CASE15)},
	{},
	{MASK_CASE17_CHUNK >> (0 * BITS_CASE17), MASK_CASE17_CHUNK >> (1 * BITS_CASE17), MASK_CASE17_CHUNK >> (2 * BITS_CASE17), MASK_CASE17_CHUNK >> (3 * BITS_CASE17), MASK_CASE17_CHUNK >> (4 * BITS_CASE17)}
};
uint32_t const word::DECODER_SHIFT[17][14] = {
	{0},
	{14,0},
	{18,9,0},
	{21,14,7,0},
	{24,20,16,12,8,4,0},
	{24,21,18,15,12,9,6,3,0},
	{26,24,22,20,18,16,14,12,10,8,6,4,2,0},
	{0},
	{14,0},
	{18,9,0},
	{21,14,7,0},
	{24,20,16,12,8,4,0},
	{24,21,18,15,12,9,6,3,0},
	{26,24,22,20,18,16,14,12,10,8,6,4,2,0},
	{20,15,10,5,0},
	{},
	{20,15,10,5,0},
};


/*
 * S18 Vector
 */
template<uint16_t b_s, class vector_type>
class vector
{
	public:
		friend class access_support<b_s>;
		friend class rank_support<0, b_s>;
		friend class rank_support<1, b_s>;
		friend class select_support<0, b_s>;
		friend class select_support<1, b_s>;

		typedef typename vector_type::iterator       iterator_type;
		typedef typename vector_type::const_iterator const_iterator_type;
		typedef typename vector_type::size_type      size_type;

	private:
		uint64_t       m_ones;        // 1 bits in original sequence
		uint64_t       m_size;        // Lenth of original bit vector
		uint64_t       s18_seq_size;  // Count of S18 words
		int_vector<32> s18_seq;       // Vector of S18 words
		int_vector<>   idx_bits;      // Total bits before block
		int_vector<>   idx_ones;      // Total 1 bits before block
		int_vector<>   l2_bits;
		int_vector<>   l2_ones;
		uint64_t       l2_bits_div;
		uint64_t       l2_ones_div;

	public:
		/* Default constructor */
		vector(void)=delete;

		/* Copy constructor */
		vector(vector const &other) /* copy */
			: m_ones(other.m_ones)
			, m_size(other.m_size)
			, s18_seq_size(other.s18_seq_size)
			, s18_seq(other.s18_seq)
			, idx_bits(other.idx_bits)
			, idx_ones(other.idx_ones)
			, l2_bits(other.l2_bits)
			, l2_ones(other.l2_ones)
			, l2_bits_div(other.l2_bits_div)
			, l2_ones_div(other.l2_ones_div)
		{} /* end vector::vector */

		/* Move constructor */
		vector(vector const &&other) /* move */
		{
			*this = std::move(other);
		} /* end vector::vector */

		/* Constructor from bitvector */
		vector(bit_vector const &bv)
			: m_ones(util::cnt_one_bits(bv))
			, m_size(bv.size())
			, s18_seq_size(0)
			, s18_seq(m_ones, 0)
			, idx_bits(m_ones / b_s + 2, 0)
			, idx_ones(m_ones / b_s + 2, 0)
			, l2_bits(0, 0)
			, l2_ones(0, 0)
			, l2_bits_div(1)
			, l2_ones_div(1)
		{
			/* Get absolute positions */
			int_vector<64> absp = int_vector<64>(m_ones, 0);
			for (uint64_t i = 0, j = 0; i < m_size; i++)
				if (bv[i]) absp[j++] = i;

			/* Get gaps from absolute positions */
			int_vector<32> gaps = int_vector<32>(m_ones, 0);
			for (uint64_t i = 1; i < m_ones; i++)
				gaps[i] = static_cast<uint32_t>(absp[i] - absp[i - 1]);
			gaps[0] = static_cast<uint32_t>(absp[0]) + 1;

			/* Indexes indices */
			uint64_t size_idx_bits = 1;
			uint64_t size_idx_ones = 1;

			/* Encode gaps into s18 words */
			int_vector<32>::const_iterator gap = gaps.begin();
			int_vector<32>::const_iterator const begin = gaps.begin();
			int_vector<32>::const_iterator const end = gaps.end();
			while (std::distance(gap, end) > 0) {
				for (uint64_t i = 0; i < b_s; i++)
					gap = pack_word(gap, end);

				uint64_t gaps_encoded = std::distance(begin, gap);
				idx_bits[size_idx_bits++] = absp[gaps_encoded - 1] + 1;
				idx_ones[size_idx_ones++] = gaps_encoded;
			}

			/* Get rid of extra unused space */
			s18_seq.resize(s18_seq_size);
			idx_bits.resize(size_idx_bits);
			idx_ones.resize(size_idx_ones);

			/* Build L2 index */
			uint64_t size_l2 = size_idx_bits;
			l2_bits.resize(size_l2);
			l2_ones.resize(size_l2);

			l2_bits_div = m_size / size_l2 + (m_size % size_l2 != 0);
			for (uint64_t i = 0; i < size_l2; i++) {
				auto it = std::upper_bound(idx_bits.begin(), idx_bits.end(), i * l2_bits_div);
				l2_bits[i] = std::distance(idx_bits.begin(), it);
			}

			l2_ones_div = (m_ones + 1) / size_l2 + ((m_ones + 1) % size_l2 != 0);
			for (uint64_t i = 0; i < size_l2; i++) {
				auto it = std::upper_bound(idx_ones.begin(), idx_ones.end(), i * l2_ones_div);
				l2_ones[i] = std::distance(idx_ones.begin(), it);
			}

			util::bit_compress(idx_bits);
			util::bit_compress(idx_ones);
			util::bit_compress(l2_bits);
			util::bit_compress(l2_ones);
		} /* end vector::vector */

		uint64_t size(void) const
		{
			return m_size;
		}

		uint64_t slow_access(uint64_t const key) const
		{
			return find_block_nth(
				s18_seq.begin(),
				s18_seq.end(),
				key
			);
		}

		uint64_t operator[](uint64_t const key) const
		{
			uint64_t pos = l2_bits[key / l2_bits_div] - 1;
			return find_block_nth(
				s18_seq.begin() + pos * b_s,
				s18_seq.end(),
				key - idx_bits[pos]
			);
		}

		int_vector<32> const &data(void) const
		{
			return s18_seq;
		}

		uint64_t serialize(std::ostream& out, structure_tree_node* v=nullptr, std::string name="") const
		{
			structure_tree_node* child = structure_tree::add_child(v, name, util::class_name(*this));

			uint64_t written_bytes = 0;
			written_bytes += write_member(m_ones, out, child, "m_ones");
			written_bytes += write_member(m_size, out, child, "m_size");
			written_bytes += write_member(s18_seq_size, out, child, "s18_seq_size");
			written_bytes += write_member(l2_bits_div, out, child, "l2_bits_div");
			written_bytes += write_member(l2_ones_div, out, child, "l2_ones_div");

			written_bytes += s18_seq.serialize(out, child, "s18_seq");
			written_bytes += idx_bits.serialize(out, child, "idx_bits");
			written_bytes += idx_ones.serialize(out, child, "idx_ones");
			written_bytes += l2_bits.serialize(out, child, "l2_bits");
			written_bytes += l2_ones.serialize(out, child, "l2_ones");

			structure_tree::add_size(child, written_bytes);

			return written_bytes;
		}

	private:
		uint64_t find_block_nth(int_vector<32>::const_iterator const begin, int_vector<32>::const_iterator const end, uint64_t target_accum) const
		{
			int_vector<32>::const_iterator gaps = begin;
			uint64_t accum = -1;

			for (; std::distance(gaps, end) > 0; gaps++) {
				word w(*gaps);
				auto const [_case, leading_1s, len] = w.metadata();

				if (leading_1s and (accum += leading_1s) >= target_accum)
					return 1;

				for (uint64_t i = 0; i < len; i++) {
					uint64_t wi = w.access_fast(i, _case);
					if (wi == 0) break; /* Word was not full */

					accum += wi;
					if (accum == target_accum) return 1;
					if (accum > target_accum) return 0;
				}
			}

			return 0;
		}

		inline int_vector<32>::const_iterator pack_word(int_vector<32>::const_iterator const begin, int_vector<32>::const_iterator const end)
		{
			/* Early return */
			if (std::distance(begin, end) <= 0)
				return begin;

			/* Pointer to gaps */
			int_vector<32>::const_iterator gaps = begin;

			/* Create and pack word */
			word w = word();
			while (std::distance(gaps, end) > 0 and w.add_if_enough_space(*gaps)) gaps++;
			s18_seq[s18_seq_size++] = w.pack();

			/* Return iterator to the very next compressable gap */
			return gaps;
		}
};


template<uint16_t b_s, class vector_type>
class access_support
{
	private:
		vector<b_s, vector_type> const &bv;
	public:
		access_support(void)=delete;
		access_support(vector<b_s, vector_type> &cv)
			: bv(cv)
		{}
		uint64_t operator()(uint64_t const key) const { return bv[key]; }

};

template<uint8_t q, uint16_t b_s, class vector_type>
class rank_support
{
	static_assert(q < 2, "rank_support: bit pattern must be `0` or `1`");
	private:
		vector<b_s, vector_type> const &bv;

		typedef typename vector_type::iterator       iterator_type;
		typedef typename vector_type::const_iterator const_iterator_type;

	private:
		uint64_t rank0(uint64_t const key) const
		{
			return key - rank1(key);
		}

		uint64_t rank1(uint64_t const key) const
		{
			uint64_t pos = bv.l2_bits[key / bv.l2_bits_div] - 1;
			return bv.idx_ones[pos] + find_block_nth(
				bv.s18_seq.begin() + pos * b_s,
				bv.s18_seq.end(),
				key - bv.idx_bits[pos]
			);
		}

		uint64_t find_block_nth(int_vector<32>::const_iterator const begin, int_vector<32>::const_iterator const end, uint64_t target_accum) const
		{
			int_vector<32>::const_iterator gaps = begin;
			uint64_t accum = -1;
			uint64_t one_cnt = 0;

			for (; std::distance(gaps, end) > 0; gaps++) {
				word w(*gaps);
				auto const [_case, leading_1s, len] = w.metadata();

				if (accum + 1 + leading_1s >= target_accum + 1)
					return one_cnt + target_accum - accum - 1;

				accum += leading_1s;
				one_cnt += leading_1s;

				for (uint64_t i = 0; i < len; i++, one_cnt++) {
					uint64_t wi = w.access_fast(i, _case);
					if (wi == 0) break; /* Word was not full */

					accum += wi;
					if (accum >= target_accum) return one_cnt;
				}
			}

			return one_cnt;
		}
	public:
		rank_support(void)=delete;
		rank_support(vector<b_s, vector_type> &cv)
			: bv(cv)
		{}
		uint64_t operator()(uint64_t const key) const
		{
			return q ? rank1(key) : rank0(key);
		}
};

template<uint8_t q, uint16_t b_s, class vector_type>
class select_support
{
	static_assert(q < 2, "select_support: bit pattern must be `0` or `1`");
	private:
		vector<b_s, vector_type> const &bv;

		typedef typename vector_type::iterator       iterator_type;
		typedef typename vector_type::const_iterator const_iterator_type;

	private:
		uint64_t select0(uint64_t const key) const
		{
			return key;
		}

		uint64_t select1(uint64_t const key) const
		{
			uint64_t pos = bv.l2_ones[key / bv.l2_ones_div] - 1;
			return bv.idx_bits[pos] + partial_sum(
				bv.s18_seq.begin() + pos * b_s,
				bv.s18_seq.end(),
				key - bv.idx_ones[pos]
			);
		}

		uint64_t partial_sum(int_vector<32>::const_iterator const begin, int_vector<32>::const_iterator const end, uint64_t counter) const
		{
			int_vector<32>::const_iterator gaps = begin;
			uint64_t accum = 0;

			for (; std::distance(gaps, end) > 0 and counter; gaps++) {
				word w(*gaps);
				auto const [_case, leading_1s, len] = w.metadata();

				accum += std::min(counter, leading_1s);
				counter -= std::min(counter, leading_1s);

				for (uint64_t i = 0; i < len and counter; i++, counter--) {
					uint64_t wi = w.access_fast(i, _case);
					if (wi == 0) break; /* Word was not full */
					accum += wi;
				}
			}

#if DEBUG
			if (counter)
				throw std::runtime_error("select_support_s17::partial_sum: vector consumed before target counter");
#endif
			return accum;
		}
	public:
		select_support(void)=delete;
		select_support(vector<b_s, vector_type> &cv)
			: bv(cv)
		{}
		uint64_t operator()(uint64_t const key) const
		{
			return q ? select1(key) : select0(key);
		}
};

} /* namespace s18 */
} /* namespace sdsl */

#endif

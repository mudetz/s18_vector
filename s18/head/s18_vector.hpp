#ifndef INCLUDED_SDSL_S18_VECTOR_V2
#define INCLUDED_SDSL_S18_VECTOR_V2

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>

#include <sdsl/int_vector.hpp>
#include <sdsl/util.hpp>

#include "constants.hpp"


namespace sdsl
{
/*
 * Forward declarations
 */

/* Access */
template<uint8_t q = 1, uint16_t b_s = 256, typename vector_type = int_vector<32>>
class access_support_s18;

/* Rank */
template<uint8_t q = 1, uint16_t b_s = 256, typename vector_type = int_vector<32>>
class rank_support_s18;

/* Select */
template<uint8_t q = 1, uint16_t b_s = 256, typename vector_type = int_vector<32>>
class select_support_s18;

/* S18 word */
class s18_word;

/* S18 vector */
template<uint16_t b_s = 256, typename vector_type = int_vector<32>>
class s18_vector;


/*
 * S18 word
 */
class s18_word
{
	private:
		bool                  already_packed;
		bool                  processing_lead_1s;
		uint32_t              leading_1s;
		std::vector<uint32_t> pending_gaps;

		static size_t const BIT_PAD[33];
	public:
		uint32_t value;
		size_t   chunk_size;
		s18_word(void)
			: already_packed(false)
			, processing_lead_1s(true)
			, leading_1s(0)
			, pending_gaps()
			, value(0)
			, chunk_size(1)
		{}

		s18_word(uint32_t const word)
			: already_packed(true)
			, processing_lead_1s(false)
			, leading_1s(0)
			, pending_gaps()
			, value(word)
			, chunk_size(0)
		{}

	public:
		std::pair<uint32_t, uint32_t> split(void) const
		{
			switch (value & MASK_HEADER4) {
				case CASE01: return std::make_pair(CASE01, (value & MASK_BODY4));
				case CASE02: return std::make_pair(CASE02, (value & MASK_BODY4));
				case CASE03: return std::make_pair(CASE03, (value & MASK_BODY4));
				case CASE04: return std::make_pair(CASE04, (value & MASK_BODY4));
				case CASE05: return std::make_pair(CASE05, (value & MASK_BODY4));
				case CASE06: return std::make_pair(CASE06, (value & MASK_BODY4));
				case CASE07: return std::make_pair(CASE07, (value & MASK_BODY4));
				case CASE08: return std::make_pair(CASE08, (value & MASK_BODY4));
				case CASE09: return std::make_pair(CASE09, (value & MASK_BODY4));
				case CASE10: return std::make_pair(CASE10, (value & MASK_BODY4));
				case CASE11: return std::make_pair(CASE11, (value & MASK_BODY4));
				case CASE12: return std::make_pair(CASE12, (value & MASK_BODY4));
				case CASE13: return std::make_pair(CASE13, (value & MASK_BODY4));
				case CASE14: return std::make_pair(CASE14, (value & MASK_BODY4));
				case CASE15: return std::make_pair(CASE15, (value & MASK_BODY4));
				default: switch (value & MASK_HEADER5) {
					case CASE16: return std::make_pair(CASE16, (value & MASK_BODY5));
					case CASE17: return std::make_pair(CASE17, (value & MASK_BODY5));
					default: throw std::invalid_argument("s18_word::split: Invalid case");
				}
			}
		}

		uint32_t header(void) const { return split().first; }
		uint32_t body(void) const { return split().second; }

		size_t size(void) const {
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
					default: throw std::invalid_argument("s18_word::size: Invalid case");
				}
			}
		}

		uint32_t operator[](size_t key) const
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
					default: throw std::invalid_argument("s18_word::operator[]: Invalid case");
				}
			}
		}

		bool add_if_enough_space(uint32_t gap)
		{
			assert(not already_packed);
			processing_lead_1s = processing_lead_1s and gap == 1;

			if (processing_lead_1s) {
				leading_1s += 1;
				return true;
			}

			if (leading_1s < 28) while (leading_1s) {
				pending_gaps.push_back(1);
				leading_1s--;
			}
			else if (leading_1s > 28) return false;

			size_t new_pending_size = pending_gaps.size() + 1;
			size_t gap_size = BIT_PAD[bits::hi(gap) + 1];
			size_t new_chunk_size = std::max(gap_size, chunk_size);
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
			if (chunk_size == 1)
				return (value = CASE16 | leading_1s);

			for (uint32_t gap : pending_gaps) {
				value <<= chunk_size;
				value |= gap;
			}
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
				default: throw std::invalid_argument("s18_word::pack: Invalid case");
			}

			return value;
		}

};

size_t const s18_word::BIT_PAD[33] = { 1, 1, 2, 3, 4, 5, 7, 7, 9, 9, 14, 14, 14, 14, 14, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28 };

/*
 * S18 Vector
 */
template<uint16_t b_s, typename vector_type>
class s18_vector
{
	public:
		friend class access_support_s18<0, b_s>;
		friend class access_support_s18<1, b_s>;
		friend class rank_support_s18<0, b_s>;
		friend class rank_support_s18<1, b_s>;
		friend class select_support_s18<0, b_s>;
		friend class select_support_s18<1, b_s>;
	private:
		size_t         m_ones;        // 1 bits in original sequence
		size_t         m_size;        // Lenth of original bit vector
		size_t         s18_seq_size;  // Count of S18 words
		int_vector<32> s18_seq;       // Vector of S18 words
		vector_type    idx_words;     // Block leading words
		vector_type    idx_bits;   // Total bits before block

		typedef typename vector_type::iterator       iterator_type;
		typedef typename vector_type::const_iterator const_iterator_type;

	public:
		/* Default constructor */
		s18_vector(void)=delete;

		/* Copy constructor */
		s18_vector(s18_vector const &other) /* copy */
			: m_ones(other.m_ones)
			, m_size(other.m_size)
			, s18_seq_size(other.s18_seq_size)
			, s18_seq(other.s18_seq)
			, idx_words(other.idx_words)
			, idx_bits(other.idx_bits)
		{} /* end s18_vector::s18_vector */

		/* Move constructor */
		s18_vector(s18_vector const &&other) /* move */
		{
			*this = std::move(other);
		} /* end s18_vector::s18_vector */

		/* Constructor from bitvector */
		s18_vector(bit_vector const &bv)
			: m_ones(util::cnt_one_bits(bv))
			, m_size(bv.size())
			, s18_seq_size(0)
			, s18_seq(m_ones, 0)
			, idx_words((m_ones / b_s) + (m_ones % b_s ? 1 : 0), 0)
			, idx_bits((m_ones / b_s) + (m_ones % b_s ? 1 : 0), 0)
		{
			/* Compression will be done in blks blocks */
			size_t blks = (m_ones / b_s) + (m_ones % b_s ? 1 : 0);

			/* Get absolute positions */
			vector_type absp = vector_type(m_ones, 0);
			for (size_t i = 0, j = 0; i < m_size; i++)
				if (bv[i]) absp[j++] = (uint32_t)i;

			/* Get gaps from absolute positions */
			vector_type gaps = vector_type(m_ones, 0);
			for (size_t i = 1; i < m_ones; i++)
				gaps[i] = absp[i] - absp[i - 1];
			gaps[0] = absp[0] + 1;

			/* Fill indexes */
			for (size_t i = 1; i < idx_bits.size(); i++)
				idx_bits[i] = absp[b_s * i - 1] + 1;
			idx_bits[0] = 0;

			/* Encode blocks into multiple S18 words each */
			for (size_t i = 0; i < blks; i++) {
				auto from = gaps.begin() + (i * b_s);
				auto to = std::min(gaps.begin() + ((i + 1) * b_s), gaps.end());
				pack_block(from, to);
				idx_words[i] = (uint32_t)s18_seq_size;
			}

			/* Get rid of extra unused space */
			s18_seq.resize(s18_seq_size);
			util::bit_compress(idx_words);
			util::bit_compress(idx_bits);
		} /* end s18_vector::s18_vector */

		size_t size(void)
		{
			return m_size;
		}

		uint32_t operator[](size_t const key) const
		{
#if 0
			auto block_to_unpack = std::lower_bound(idx_bits.begin(), idx_bits.end(), key);
			size_t position_in_idx_for_unpack = std::distance(idx_bits.begin(), block_to_unpack);

			size_t start =  idx_words[position_in_idx_for_unpack];
			size_t end =  idx_words[position_in_idx_for_unpack + 1];

			return find_block_nth(
				s18_seq.begin() + start,
				s18_seq.begin() + end,
				(uint32_t)key - *block_to_unpack
			);
#endif
			return find_block_nth(
				s18_seq.begin(),
				s18_seq.end(),
				(uint32_t)key + 1
			);
		}

	private:
		uint32_t find_block_nth(const_iterator_type const begin, const_iterator_type const end, uint32_t target_accum) const
		{
			const_iterator_type gaps = begin;
			uint32_t accum = 0;

			for (; std::distance(gaps, end) > 0; gaps++) {
				s18_word word = s18_word(*gaps);
				size_t len = word.size();

				for (size_t i = 0; i < len; i++) {
					accum += word[i];
					if (accum == target_accum) return 1;
					if (accum > target_accum) return 0;
				}
			}

			throw std::runtime_error("s18_vector::find_block_nth: key not found within block range");
		}

		inline void pack_block(const_iterator_type const begin, const_iterator_type const end)
		{
			/* Pointer to gaps */
			const_iterator_type gaps = begin;
			while (std::distance(gaps, end) > 0)
				gaps = pack_word(gaps, end);
		}

		inline const_iterator_type pack_word(const_iterator_type const begin, const_iterator_type const end)
		{
			/* Pointer to gaps */
			const_iterator_type gaps = begin;

			/* Create and pack word */
			s18_word word = s18_word();
			while (std::distance(gaps, end) > 0 and word.add_if_enough_space(*gaps)) gaps++;
			s18_seq[s18_seq_size++] = word.pack();

			/* Return iterator to the very next compressable gap */
			return gaps;
		}

#ifdef TEST_S18
	public:
		int_vector<32> const &debug__s18_seq(void) const { return s18_seq; }
		uint32_t debug__first_word_nth_gap(size_t n) const { return s18_word(*s18_seq.begin())[n]; }
#endif
};

}

#endif

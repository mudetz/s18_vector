#include <benchmark/benchmark.h>
#include <sdsl/int_vector.hpp>
#include <sdsl/rrr_vector.hpp>
#include <sdsl/sd_vector.hpp>
#include <sdsl/util.hpp>
#include "s18_vector.hpp"
#include "s9_vector.hpp"

#define SIZE 1000000

static double const DENSITY[] = {.97, .5, .03};


/*
 * ACCESS
 */
template <class BV>
static void BM_access_bv(benchmark::State& state) {
	std::random_device g;
	std::geometric_distribution<uint32_t> d(DENSITY[state.range(0)]);
	BV bv = sdsl::bit_vector(SIZE, 0);
	size_t i = d(g) + 1;
	while (i < bv.size()) {
		bv[i] = 1;
		i += d(g) + 1;
	}

	std::uniform_int_distribution<int> idx(0, SIZE - 1);
	for (auto _ : state)
		bv[idx(g)];

	benchmark::DoNotOptimize(bv.data());
}
BENCHMARK_TEMPLATE(BM_access_bv, sdsl::bit_vector)->DenseRange(0,2,1);

template <class S9V>
static void BM_access_s9(benchmark::State& state) {
	std::random_device g;
	std::geometric_distribution<uint32_t> d(DENSITY[state.range(0)]);
	sdsl::bit_vector bv = sdsl::bit_vector(SIZE, 0);
	size_t i = d(g) + 1;
	while (i < bv.size()) {
		bv[i] = 1;
		i += d(g) + 1;
	}

	S9V s9(bv);
	std::uniform_int_distribution<int> idx(0, SIZE - 1);
	for (auto _ : state)
		s9[idx(g)];

	benchmark::DoNotOptimize(s9.get_seq());
}
BENCHMARK_TEMPLATE(BM_access_s9, sdsl::s9_vector<8>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_s9, sdsl::s9_vector<16>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_s9, sdsl::s9_vector<32>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_s9, sdsl::s9_vector<64>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_s9, sdsl::s9_vector<128>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_s9, sdsl::s9_vector<256>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_s9, sdsl::s9_vector<512>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_s9, sdsl::s9_vector<1024>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_s9, sdsl::s9_vector<2048>)->DenseRange(0,2,1);

template <class S18V>
static void BM_access_s18(benchmark::State& state) {
	std::random_device g;
	std::geometric_distribution<uint32_t> d(DENSITY[state.range(0)]);
	sdsl::bit_vector bv = sdsl::bit_vector(SIZE, 0);
	size_t i = d(g) + 1;
	while (i < bv.size()) {
		bv[i] = 1;
		i += d(g) + 1;
	}

	S18V s18(bv);
	std::uniform_int_distribution<int> idx(0, SIZE - 1);
	for (auto _ : state)
		s18[idx(g)];

	benchmark::DoNotOptimize(s18.data());
}
BENCHMARK_TEMPLATE(BM_access_s18, sdsl::s18_vector<1>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_s18, sdsl::s18_vector<2>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_s18, sdsl::s18_vector<4>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_s18, sdsl::s18_vector<8>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_s18, sdsl::s18_vector<16>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_s18, sdsl::s18_vector<32>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_s18, sdsl::s18_vector<64>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_s18, sdsl::s18_vector<128>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_s18, sdsl::s18_vector<256>)->DenseRange(0,2,1);

template <class RRR>
static void BM_access_rrr(benchmark::State& state) {
	std::random_device g;
	std::geometric_distribution<uint32_t> d(DENSITY[state.range(0)]);
	sdsl::bit_vector bv = sdsl::bit_vector(SIZE, 0);
	size_t i = d(g) + 1;
	while (i < bv.size()) {
		bv[i] = 1;
		i += d(g) + 1;
	}

	RRR rrr(bv);
	std::uniform_int_distribution<int> idx(0, SIZE - 1);
	for (auto _ : state)
		rrr[idx(g)];

	benchmark::DoNotOptimize(rrr.begin());
}
BENCHMARK_TEMPLATE(BM_access_rrr, sdsl::rrr_vector<8>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_rrr, sdsl::rrr_vector<16>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_rrr, sdsl::rrr_vector<32>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_rrr, sdsl::rrr_vector<64>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_rrr, sdsl::rrr_vector<128>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_access_rrr, sdsl::rrr_vector<256>)->DenseRange(0,2,1);

template <class SD>
static void BM_access_sd(benchmark::State& state) {
	std::random_device g;
	std::geometric_distribution<uint32_t> d(DENSITY[state.range(0)]);
	sdsl::bit_vector bv = sdsl::bit_vector(SIZE, 0);
	size_t i = d(g) + 1;
	while (i < bv.size()) {
		bv[i] = 1;
		i += d(g) + 1;
	}

	SD sd(bv);
	std::uniform_int_distribution<int> idx(0, SIZE - 1);
	for (auto _ : state)
		sd[idx(g)];

	benchmark::DoNotOptimize(sd.begin());
}
BENCHMARK_TEMPLATE(BM_access_sd, sdsl::sd_vector<>)->DenseRange(0,2,1);


/*
 * RANK
 */
template <class BV, class RS>
static void BM_rank_bv(benchmark::State& state) {
	std::random_device g;
	std::geometric_distribution<uint32_t> d(DENSITY[state.range(0)]);
	BV bv = sdsl::bit_vector(SIZE, 0);
	size_t i = d(g) + 1;
	while (i < bv.size()) {
		bv[i] = 1;
		i += d(g) + 1;
	}

	RS rs(&bv);
	std::uniform_int_distribution<int> idx(0, SIZE - 1);
	for (auto _ : state)
		rs(idx(g));

	benchmark::DoNotOptimize(bv.data());
}
BENCHMARK_TEMPLATE(BM_rank_bv, sdsl::bit_vector, sdsl::rank_support_v<1>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_bv, sdsl::bit_vector, sdsl::rank_support_v5<1>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_bv, sdsl::bit_vector, sdsl::rank_support_scan<1>)->DenseRange(0,2,1);

template <class S9V, class RS>
static void BM_rank_s9(benchmark::State& state) {
	std::random_device g;
	std::geometric_distribution<uint32_t> d(DENSITY[state.range(0)]);
	sdsl::bit_vector bv = sdsl::bit_vector(SIZE, 0);
	size_t i = d(g) + 1;
	while (i < bv.size()) {
		bv[i] = 1;
		i += d(g) + 1;
	}

	S9V s9(bv);
	RS rs(&s9);
	std::uniform_int_distribution<int> idx(0, SIZE - 1);
	for (auto _ : state)
		rs(idx(g));

	benchmark::DoNotOptimize(s9.get_seq());
}
BENCHMARK_TEMPLATE(BM_rank_s9, sdsl::s9_vector<8>, sdsl::rank_support_s9<1,8>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_s9, sdsl::s9_vector<16>, sdsl::rank_support_s9<1,16>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_s9, sdsl::s9_vector<32>, sdsl::rank_support_s9<1,32>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_s9, sdsl::s9_vector<64>, sdsl::rank_support_s9<1,64>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_s9, sdsl::s9_vector<128>, sdsl::rank_support_s9<1,128>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_s9, sdsl::s9_vector<256>, sdsl::rank_support_s9<1,256>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_s9, sdsl::s9_vector<512>, sdsl::rank_support_s9<1,512>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_s9, sdsl::s9_vector<1024>, sdsl::rank_support_s9<1,1024>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_s9, sdsl::s9_vector<2048>, sdsl::rank_support_s9<1,2048>)->DenseRange(0,2,1);

template <class S18V, class RS>
static void BM_rank_s18(benchmark::State& state) {
	std::random_device g;
	std::geometric_distribution<uint32_t> d(DENSITY[state.range(0)]);
	sdsl::bit_vector bv = sdsl::bit_vector(SIZE, 0);
	size_t i = d(g) + 1;
	while (i < bv.size()) {
		bv[i] = 1;
		i += d(g) + 1;
	}

	S18V s18(bv);
	RS rs(s18);
	std::uniform_int_distribution<int> idx(0, SIZE - 1);
	for (auto _ : state)
		rs(idx(g));

	benchmark::DoNotOptimize(s18.data());
}
BENCHMARK_TEMPLATE(BM_rank_s18, sdsl::s18_vector<1>, sdsl::rank_support_s18<1,1>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_s18, sdsl::s18_vector<2>, sdsl::rank_support_s18<1,2>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_s18, sdsl::s18_vector<4>, sdsl::rank_support_s18<1,4>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_s18, sdsl::s18_vector<8>, sdsl::rank_support_s18<1,8>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_s18, sdsl::s18_vector<16>, sdsl::rank_support_s18<1,16>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_s18, sdsl::s18_vector<32>, sdsl::rank_support_s18<1,32>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_s18, sdsl::s18_vector<64>, sdsl::rank_support_s18<1,64>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_s18, sdsl::s18_vector<128>, sdsl::rank_support_s18<1,128>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_s18, sdsl::s18_vector<256>, sdsl::rank_support_s18<1,256>)->DenseRange(0,2,1);


template <class RRR, class RS>
static void BM_rank_rrr(benchmark::State& state) {
	std::random_device g;
	std::geometric_distribution<uint32_t> d(DENSITY[state.range(0)]);
	sdsl::bit_vector bv = sdsl::bit_vector(SIZE, 0);
	size_t i = d(g) + 1;
	while (i < bv.size()) {
		bv[i] = 1;
		i += d(g) + 1;
	}

	RRR rrr(bv);
	RS rs(&rrr);
	std::uniform_int_distribution<int> idx(0, SIZE - 1);
	for (auto _ : state)
		rs(idx(g));

	benchmark::DoNotOptimize(rrr.begin());
}
BENCHMARK_TEMPLATE(BM_rank_rrr, sdsl::rrr_vector<8>, sdsl::rank_support_rrr<1,8>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_rrr, sdsl::rrr_vector<16>, sdsl::rank_support_rrr<1,16>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_rrr, sdsl::rrr_vector<32>, sdsl::rank_support_rrr<1,32>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_rrr, sdsl::rrr_vector<64>, sdsl::rank_support_rrr<1,64>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_rrr, sdsl::rrr_vector<128>, sdsl::rank_support_rrr<1,128>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_rank_rrr, sdsl::rrr_vector<256>, sdsl::rank_support_rrr<1,256>)->DenseRange(0,2,1);


template <class SD, class RS>
static void BM_rank_sd(benchmark::State& state) {
	std::random_device g;
	std::geometric_distribution<uint32_t> d(DENSITY[state.range(0)]);
	sdsl::bit_vector bv = sdsl::bit_vector(SIZE, 0);
	size_t i = d(g) + 1;
	while (i < bv.size()) {
		bv[i] = 1;
		i += d(g) + 1;
	}

	SD sd(bv);
	RS rs(&sd);
	std::uniform_int_distribution<int> idx(0, SIZE - 1);
	for (auto _ : state)
		rs(idx(g));

	benchmark::DoNotOptimize(sd.begin());
}
BENCHMARK_TEMPLATE(BM_rank_sd, sdsl::sd_vector<>, sdsl::rank_support_sd<1>)->DenseRange(0,2,1);


/*
 * SELECT
 */
template <class BV, class SS>
static void BM_select_bv(benchmark::State& state) {
	std::random_device g;
	std::geometric_distribution<uint32_t> d(DENSITY[state.range(0)]);
	BV bv = sdsl::bit_vector(SIZE, 0);
	size_t i = d(g) + 1;
	while (i < bv.size()) {
		bv[i] = 1;
		i += d(g) + 1;
	}

	SS ss(&bv);
	int MAX = (int)sdsl::util::cnt_one_bits(bv);

	std::uniform_int_distribution<int> idx(1, MAX);
	for (auto _ : state)
		ss(idx(g));

	benchmark::DoNotOptimize(bv.data());
}
BENCHMARK_TEMPLATE(BM_select_bv, sdsl::bit_vector, sdsl::select_support_mcl<1>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_bv, sdsl::bit_vector, sdsl::select_support_scan<1>)->DenseRange(0,2,1);

template <class S9V, class SS>
static void BM_select_s9(benchmark::State& state) {
	std::random_device g;
	std::geometric_distribution<uint32_t> d(DENSITY[state.range(0)]);
	sdsl::bit_vector bv = sdsl::bit_vector(SIZE, 0);
	size_t i = d(g) + 1;
	while (i < bv.size()) {
		bv[i] = 1;
		i += d(g) + 1;
	}
	int MAX = (int)sdsl::util::cnt_one_bits(bv);

	S9V s9(bv);
	SS ss(&s9);
	std::uniform_int_distribution<int> idx(1, MAX);
	for (auto _ : state)
		ss(idx(g));

	benchmark::DoNotOptimize(s9.get_seq());
}
BENCHMARK_TEMPLATE(BM_select_s9, sdsl::s9_vector<8>, sdsl::select_support_s9<1,8>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_s9, sdsl::s9_vector<16>, sdsl::select_support_s9<1,16>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_s9, sdsl::s9_vector<32>, sdsl::select_support_s9<1,32>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_s9, sdsl::s9_vector<64>, sdsl::select_support_s9<1,64>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_s9, sdsl::s9_vector<128>, sdsl::select_support_s9<1,128>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_s9, sdsl::s9_vector<256>, sdsl::select_support_s9<1,256>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_s9, sdsl::s9_vector<512>, sdsl::select_support_s9<1,512>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_s9, sdsl::s9_vector<1024>, sdsl::select_support_s9<1,1024>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_s9, sdsl::s9_vector<2048>, sdsl::select_support_s9<1,2048>)->DenseRange(0,2,1);

template <class S18V, class SS>
static void BM_select_s18(benchmark::State& state) {
	std::random_device g;
	std::geometric_distribution<uint32_t> d(DENSITY[state.range(0)]);
	sdsl::bit_vector bv = sdsl::bit_vector(SIZE, 0);
	size_t i = d(g) + 1;
	while (i < bv.size()) {
		bv[i] = 1;
		i += d(g) + 1;
	}
	int MAX = (int)sdsl::util::cnt_one_bits(bv);

	S18V s18(bv);
	SS ss(s18);
	std::uniform_int_distribution<int> idx(0, MAX - 1);
	for (auto _ : state)
		ss(idx(g));

	benchmark::DoNotOptimize(s18.data());
}
BENCHMARK_TEMPLATE(BM_select_s18, sdsl::s18_vector<1>, sdsl::select_support_s18<1,1>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_s18, sdsl::s18_vector<2>, sdsl::select_support_s18<1,2>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_s18, sdsl::s18_vector<4>, sdsl::select_support_s18<1,4>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_s18, sdsl::s18_vector<8>, sdsl::select_support_s18<1,8>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_s18, sdsl::s18_vector<16>, sdsl::select_support_s18<1,16>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_s18, sdsl::s18_vector<32>, sdsl::select_support_s18<1,32>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_s18, sdsl::s18_vector<64>, sdsl::select_support_s18<1,64>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_s18, sdsl::s18_vector<128>, sdsl::select_support_s18<1,128>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_s18, sdsl::s18_vector<256>, sdsl::select_support_s18<1,256>)->DenseRange(0,2,1);


template <class RRR, class SS>
static void BM_select_rrr(benchmark::State& state) {
	std::random_device g;
	std::geometric_distribution<uint32_t> d(DENSITY[state.range(0)]);
	sdsl::bit_vector bv = sdsl::bit_vector(SIZE, 0);
	size_t i = d(g) + 1;
	while (i < bv.size()) {
		bv[i] = 1;
		i += d(g) + 1;
	}
	int MAX = (int)sdsl::util::cnt_one_bits(bv);

	RRR rrr(bv);
	SS ss(&rrr);
	std::uniform_int_distribution<int> idx(1, MAX);
	for (auto _ : state)
		ss(idx(g));

	benchmark::DoNotOptimize(rrr.begin());
}
BENCHMARK_TEMPLATE(BM_select_rrr, sdsl::rrr_vector<8>, sdsl::select_support_rrr<1,8>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_rrr, sdsl::rrr_vector<16>, sdsl::select_support_rrr<1,16>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_rrr, sdsl::rrr_vector<32>, sdsl::select_support_rrr<1,32>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_rrr, sdsl::rrr_vector<64>, sdsl::select_support_rrr<1,64>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_rrr, sdsl::rrr_vector<128>, sdsl::select_support_rrr<1,128>)->DenseRange(0,2,1);
BENCHMARK_TEMPLATE(BM_select_rrr, sdsl::rrr_vector<256>, sdsl::select_support_rrr<1,256>)->DenseRange(0,2,1);


template <class SD, class SS>
static void BM_select_sd(benchmark::State& state) {
	std::random_device g;
	std::geometric_distribution<uint32_t> d(DENSITY[state.range(0)]);
	sdsl::bit_vector bv = sdsl::bit_vector(SIZE, 0);
	size_t i = d(g) + 1;
	while (i < bv.size()) {
		bv[i] = 1;
		i += d(g) + 1;
	}
	int MAX = (int)sdsl::util::cnt_one_bits(bv);

	SD sd(bv);
	SS ss(&sd);
	std::uniform_int_distribution<int> idx(1, MAX);
	for (auto _ : state)
		ss(idx(g));

	benchmark::DoNotOptimize(sd.begin());
}
BENCHMARK_TEMPLATE(BM_select_sd, sdsl::sd_vector<>, sdsl::select_support_sd<1>)->DenseRange(0,2,1);

BENCHMARK_MAIN();

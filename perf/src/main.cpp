#include <benchmark/benchmark.h>
#include <sdsl/int_vector.hpp>
#include <sdsl/rrr_vector.hpp>
#include <sdsl/sd_vector.hpp>
#include <sdsl/hyb_vector.hpp>
#include <sdsl/util.hpp>
#include "s18_vector.hpp"
#include "s9_vector.hpp"

#define GAPS 1000000
static unsigned int SIZE[36] = { 0 };

void create_bv(sdsl::bit_vector &bv, unsigned int lambda, unsigned int x, double p)
{
	std::random_device rd;
	std::uniform_real_distribution<double> rr(0.,1.);
	std::uniform_int_distribution<size_t> ri(1, x);
	std::poisson_distribution<size_t> rp(lambda - 1);

	size_t pos = 0;
	for (size_t i = 0; i < GAPS; i++) {
		if (rr(rd) < p) {
			size_t len = ri(rd);
			for (size_t j = 0; j < len; j++)
				bv[pos++] = 1;
		}
		size_t len = rp(rd);
		for (size_t j = 0; j < len; j++)
			bv[pos++] = 0;
		bv[pos++] = 1;
	}
	bv.resize(pos);
}

sdsl::bit_vector &test_bv(int c)
{
	static int const lambda[] = {4, 7, 31, 127};
	static int const x[] = {1000, 10000, 100000};
	static double const p[] = {0.01, 0.001, 0.0001};
	static bool created = false;

	static sdsl::bit_vector bvs[] = {
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
		sdsl::bit_vector((130 + 1000) * GAPS, 0),
	};

	for (size_t i = 0; !created and i < 4; i++) {
		for (size_t j = 0; j < 3; j++) {
			for (size_t k = 0; k < 3; k++) {
				create_bv(bvs[9*i+3*j+k], lambda[i], x[j], p[k]);
				SIZE[9*i+3*j+k] = static_cast<unsigned int>(bvs[9*i+3*j+k].size());
			}
		}
	}

	created = true;
	return bvs[c];
}

/*
 * ACCESS
 */
template <class BV>
static void BM_access_bv(benchmark::State& state) {
	BV &bv = test_bv(static_cast<int>(state.range(0)));

	std::random_device g;
	std::uniform_int_distribution<int> idx(0, SIZE[state.range(0)] - 1);
	for (auto _ : state)
		bv[idx(g)];

	benchmark::DoNotOptimize(bv.data());
	state.counters["bits"] = bv.size();
	state.counters["size"] = size_in_mega_bytes(bv);
	state.counters["comp"] = size_in_mega_bytes(bv);
}
BENCHMARK_TEMPLATE(BM_access_bv, sdsl::bit_vector)->DenseRange(0,35,1);

template <class S9V>
static void BM_access_s9(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));

	S9V s9(bv);
	std::random_device g;
	std::uniform_int_distribution<int> idx(0, SIZE[state.range(0)] - 1);
	for (auto _ : state)
		s9[idx(g)];

	benchmark::DoNotOptimize(s9.get_seq());
	state.counters["bits"] = bv.size();
	state.counters["size"] = size_in_mega_bytes(bv);
	state.counters["comp"] = size_in_mega_bytes(s9);
}
BENCHMARK_TEMPLATE(BM_access_s9, sdsl::s9_vector<8>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_access_s9, sdsl::s9_vector<16>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_access_s9, sdsl::s9_vector<32>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_access_s9, sdsl::s9_vector<64>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_access_s9, sdsl::s9_vector<128>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_access_s9, sdsl::s9_vector<256>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_access_s9, sdsl::s9_vector<512>)->DenseRange(0,35,1);

template <class S18V>
static void BM_access_s18(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));

	S18V s18(bv);
	std::random_device g;
	std::uniform_int_distribution<int> idx(0, SIZE[state.range(0)] - 1);
	for (auto _ : state)
		s18[idx(g)];

	benchmark::DoNotOptimize(s18.data());
	state.counters["bits"] = bv.size();
	state.counters["size"] = size_in_mega_bytes(bv);
	state.counters["comp"] = size_in_mega_bytes(s18);
}
BENCHMARK_TEMPLATE(BM_access_s18, sdsl::s18::vector<1>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_access_s18, sdsl::s18::vector<2>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_access_s18, sdsl::s18::vector<4>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_access_s18, sdsl::s18::vector<8>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_access_s18, sdsl::s18::vector<16>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_access_s18, sdsl::s18::vector<32>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_access_s18, sdsl::s18::vector<64>)->DenseRange(0,35,1);

template <class RRR>
static void BM_access_rrr(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;

	RRR rrr(bv);
	std::uniform_int_distribution<int> idx(0, SIZE[state.range(0)] - 1);
	for (auto _ : state)
		rrr[idx(g)];

	benchmark::DoNotOptimize(rrr.begin());
	state.counters["bits"] = bv.size();
	state.counters["size"] = size_in_mega_bytes(bv);
	state.counters["comp"] = size_in_mega_bytes(rrr);
}
BENCHMARK_TEMPLATE(BM_access_rrr, sdsl::rrr_vector<8>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_access_rrr, sdsl::rrr_vector<16>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_access_rrr, sdsl::rrr_vector<32>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_access_rrr, sdsl::rrr_vector<64>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_access_rrr, sdsl::rrr_vector<128>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_access_rrr, sdsl::rrr_vector<256>)->DenseRange(0,35,1);

template <class SD>
static void BM_access_sd(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;

	SD sd(bv);
	std::uniform_int_distribution<int> idx(0, SIZE[state.range(0)] - 1);
	for (auto _ : state)
		sd[idx(g)];

	benchmark::DoNotOptimize(sd.begin());
	state.counters["bits"] = bv.size();
	state.counters["size"] = size_in_mega_bytes(bv);
	state.counters["comp"] = size_in_mega_bytes(sd);
}
BENCHMARK_TEMPLATE(BM_access_sd, sdsl::sd_vector<>)->DenseRange(0,35,1);

static void BM_access_hyb(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;

	sdsl::hyb_vector hv(bv);
	std::uniform_int_distribution<int> idx(0, SIZE[state.range(0)] - 1);
	for (auto _ : state)
		hv[idx(g)];

	benchmark::DoNotOptimize(hv.begin());
	state.counters["bits"] = bv.size();
	state.counters["size"] = size_in_mega_bytes(bv);
	state.counters["comp"] = size_in_mega_bytes(hv);
}
BENCHMARK(BM_access_hyb)->DenseRange(0,35,1);


/*
 * RANK
 */
template <class BV, class RS>
static void BM_rank_bv(benchmark::State& state) {
	BV &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;

	RS rs(&bv);
	std::uniform_int_distribution<int> idx(0, SIZE[state.range(0)] - 1);
	for (auto _ : state)
		rs(idx(g));

	benchmark::DoNotOptimize(bv.data());
}
BENCHMARK_TEMPLATE(BM_rank_bv, sdsl::bit_vector, sdsl::rank_support_v<1>)->DenseRange(0,35,1);
//BENCHMARK_TEMPLATE(BM_rank_bv, sdsl::bit_vector, sdsl::rank_support_v5<1>)->DenseRange(0,35,1);
//BENCHMARK_TEMPLATE(BM_rank_bv, sdsl::bit_vector, sdsl::rank_support_scan<1>)->DenseRange(0,35,1);

template <class S9V, class RS>
static void BM_rank_s9(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;

	S9V s9(bv);
	RS rs(&s9);
	std::uniform_int_distribution<int> idx(0, SIZE[state.range(0)] - 1);
	for (auto _ : state)
		rs(idx(g));

	benchmark::DoNotOptimize(s9.get_seq());
}
BENCHMARK_TEMPLATE(BM_rank_s9, sdsl::s9_vector<8>, sdsl::rank_support_s9<1,8>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_rank_s9, sdsl::s9_vector<16>, sdsl::rank_support_s9<1,16>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_rank_s9, sdsl::s9_vector<32>, sdsl::rank_support_s9<1,32>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_rank_s9, sdsl::s9_vector<64>, sdsl::rank_support_s9<1,64>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_rank_s9, sdsl::s9_vector<128>, sdsl::rank_support_s9<1,128>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_rank_s9, sdsl::s9_vector<256>, sdsl::rank_support_s9<1,256>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_rank_s9, sdsl::s9_vector<512>, sdsl::rank_support_s9<1,512>)->DenseRange(0,35,1);

template <class S18V, class RS>
static void BM_rank_s18(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;

	S18V s18(bv);
	RS rs(s18);
	std::uniform_int_distribution<int> idx(0, SIZE[state.range(0)] - 1);
	for (auto _ : state)
		rs(idx(g));

	benchmark::DoNotOptimize(s18.data());
}
BENCHMARK_TEMPLATE(BM_rank_s18, sdsl::s18::vector<1>, sdsl::s18::rank_support<1,1>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_rank_s18, sdsl::s18::vector<2>, sdsl::s18::rank_support<1,2>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_rank_s18, sdsl::s18::vector<4>, sdsl::s18::rank_support<1,4>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_rank_s18, sdsl::s18::vector<8>, sdsl::s18::rank_support<1,8>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_rank_s18, sdsl::s18::vector<16>, sdsl::s18::rank_support<1,16>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_rank_s18, sdsl::s18::vector<32>, sdsl::s18::rank_support<1,32>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_rank_s18, sdsl::s18::vector<64>, sdsl::s18::rank_support<1,64>)->DenseRange(0,35,1);


template <class RRR, class RS>
static void BM_rank_rrr(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;

	RRR rrr(bv);
	RS rs(&rrr);
	std::uniform_int_distribution<int> idx(0, SIZE[state.range(0)] - 1);
	for (auto _ : state)
		rs(idx(g));

	benchmark::DoNotOptimize(rrr.begin());
}
BENCHMARK_TEMPLATE(BM_rank_rrr, sdsl::rrr_vector<8>, sdsl::rank_support_rrr<1,8>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_rank_rrr, sdsl::rrr_vector<16>, sdsl::rank_support_rrr<1,16>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_rank_rrr, sdsl::rrr_vector<32>, sdsl::rank_support_rrr<1,32>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_rank_rrr, sdsl::rrr_vector<64>, sdsl::rank_support_rrr<1,64>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_rank_rrr, sdsl::rrr_vector<128>, sdsl::rank_support_rrr<1,128>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_rank_rrr, sdsl::rrr_vector<256>, sdsl::rank_support_rrr<1,256>)->DenseRange(0,35,1);


template <class SD, class RS>
static void BM_rank_sd(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;

	SD sd(bv);
	RS rs(&sd);
	std::uniform_int_distribution<int> idx(0, SIZE[state.range(0)] - 1);
	for (auto _ : state)
		rs(idx(g));

	benchmark::DoNotOptimize(sd.begin());
}
BENCHMARK_TEMPLATE(BM_rank_sd, sdsl::sd_vector<>, sdsl::rank_support_sd<1>)->DenseRange(0,35,1);

static void BM_rank_hyb(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;

	sdsl::hyb_vector hv(bv);
	sdsl::rank_support_hyb<1> rs(&hv);
	std::uniform_int_distribution<int> idx(0, SIZE[state.range(0)] - 1);
	for (auto _ : state)
		rs(idx(g));

	benchmark::DoNotOptimize(hv.begin());
}
BENCHMARK(BM_rank_hyb)->DenseRange(0,35,1);


/*
 * SELECT
 */
template <class BV, class SS>
static void BM_select_bv(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;

	SS ss(&bv);
	int MAX = (int)sdsl::util::cnt_one_bits(bv);

	std::uniform_int_distribution<int> idx(1, MAX);
	for (auto _ : state)
		ss(idx(g));

	benchmark::DoNotOptimize(bv.data());
}
BENCHMARK_TEMPLATE(BM_select_bv, sdsl::bit_vector, sdsl::select_support_mcl<1>)->DenseRange(0,35,1);
//BENCHMARK_TEMPLATE(BM_select_bv, sdsl::bit_vector, sdsl::select_support_scan<1>)->DenseRange(0,35,1);

template <class S9V, class SS>
static void BM_select_s9(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;
	int MAX = (int)sdsl::util::cnt_one_bits(bv);

	S9V s9(bv);
	SS ss(&s9);
	std::uniform_int_distribution<int> idx(1, MAX);
	for (auto _ : state)
		ss(idx(g));

	benchmark::DoNotOptimize(s9.get_seq());
}
BENCHMARK_TEMPLATE(BM_select_s9, sdsl::s9_vector<8>, sdsl::select_support_s9<1,8>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_select_s9, sdsl::s9_vector<16>, sdsl::select_support_s9<1,16>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_select_s9, sdsl::s9_vector<32>, sdsl::select_support_s9<1,32>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_select_s9, sdsl::s9_vector<64>, sdsl::select_support_s9<1,64>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_select_s9, sdsl::s9_vector<128>, sdsl::select_support_s9<1,128>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_select_s9, sdsl::s9_vector<256>, sdsl::select_support_s9<1,256>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_select_s9, sdsl::s9_vector<512>, sdsl::select_support_s9<1,512>)->DenseRange(0,35,1);

template <class S18V, class SS>
static void BM_select_s18(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;
	int MAX = (int)sdsl::util::cnt_one_bits(bv);

	S18V s18(bv);
	SS ss(s18);
	std::uniform_int_distribution<int> idx(0, MAX - 1);
	for (auto _ : state)
		ss(idx(g));

	benchmark::DoNotOptimize(s18.data());
}
BENCHMARK_TEMPLATE(BM_select_s18, sdsl::s18::vector<1>, sdsl::s18::select_support<1,1>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_select_s18, sdsl::s18::vector<2>, sdsl::s18::select_support<1,2>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_select_s18, sdsl::s18::vector<4>, sdsl::s18::select_support<1,4>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_select_s18, sdsl::s18::vector<8>, sdsl::s18::select_support<1,8>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_select_s18, sdsl::s18::vector<16>, sdsl::s18::select_support<1,16>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_select_s18, sdsl::s18::vector<32>, sdsl::s18::select_support<1,32>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_select_s18, sdsl::s18::vector<64>, sdsl::s18::select_support<1,64>)->DenseRange(0,35,1);


template <class RRR, class SS>
static void BM_select_rrr(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;
	int MAX = (int)sdsl::util::cnt_one_bits(bv);

	RRR rrr(bv);
	SS ss(&rrr);
	std::uniform_int_distribution<int> idx(1, MAX);
	for (auto _ : state)
		ss(idx(g));

	benchmark::DoNotOptimize(rrr.begin());
}
BENCHMARK_TEMPLATE(BM_select_rrr, sdsl::rrr_vector<8>, sdsl::select_support_rrr<1,8>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_select_rrr, sdsl::rrr_vector<16>, sdsl::select_support_rrr<1,16>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_select_rrr, sdsl::rrr_vector<32>, sdsl::select_support_rrr<1,32>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_select_rrr, sdsl::rrr_vector<64>, sdsl::select_support_rrr<1,64>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_select_rrr, sdsl::rrr_vector<128>, sdsl::select_support_rrr<1,128>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_select_rrr, sdsl::rrr_vector<256>, sdsl::select_support_rrr<1,256>)->DenseRange(0,35,1);


template <class SD, class SS>
static void BM_select_sd(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;
	int MAX = (int)sdsl::util::cnt_one_bits(bv);

	SD sd(bv);
	SS ss(&sd);
	std::uniform_int_distribution<int> idx(1, MAX);
	for (auto _ : state)
		ss(idx(g));

	benchmark::DoNotOptimize(sd.begin());
}
BENCHMARK_TEMPLATE(BM_select_sd, sdsl::sd_vector<>, sdsl::select_support_sd<1>)->DenseRange(0,35,1);


/*
 * SUCCESSOR
 */
template <class BV, class RS, class SS>
static void BM_successor_bv(benchmark::State& state) {
	BV &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;

	RS rs(&bv);
	SS ss(&bv);
	std::uniform_int_distribution<int> idx(0, SIZE[state.range(0)] - 1);
	for (auto _ : state)
		ss(rs(idx(g)) + 1);

	benchmark::DoNotOptimize(bv.data());
}
BENCHMARK_TEMPLATE(BM_successor_bv, sdsl::bit_vector, sdsl::rank_support_v<1>, sdsl::select_support_mcl<1>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_bv, sdsl::bit_vector, sdsl::rank_support_v5<1>, sdsl::select_support_mcl<1>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_bv, sdsl::bit_vector, sdsl::rank_support_scan<1>, sdsl::select_support_mcl<1>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_bv, sdsl::bit_vector, sdsl::rank_support_scan<1>, sdsl::select_support_scan<1>)->DenseRange(0,35,1);

template <class S9V, class RS, class SS>
static void BM_successor_s9(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;

	S9V s9(bv);
	RS rs(&s9);
	SS ss(&s9);
	std::uniform_int_distribution<int> idx(0, SIZE[state.range(0)] - 1);
	for (auto _ : state)
		ss(rs(idx(g)) + 1);

	benchmark::DoNotOptimize(s9.get_seq());
}
BENCHMARK_TEMPLATE(BM_successor_s9, sdsl::s9_vector<8>, sdsl::rank_support_s9<1,8>, sdsl::select_support_s9<1,8>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_s9, sdsl::s9_vector<16>, sdsl::rank_support_s9<1,16>, sdsl::select_support_s9<1,16>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_s9, sdsl::s9_vector<32>, sdsl::rank_support_s9<1,32>, sdsl::select_support_s9<1,32>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_s9, sdsl::s9_vector<64>, sdsl::rank_support_s9<1,64>, sdsl::select_support_s9<1,64>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_s9, sdsl::s9_vector<128>, sdsl::rank_support_s9<1,128>, sdsl::select_support_s9<1,128>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_s9, sdsl::s9_vector<256>, sdsl::rank_support_s9<1,256>, sdsl::select_support_s9<1,256>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_s9, sdsl::s9_vector<512>, sdsl::rank_support_s9<1,512>, sdsl::select_support_s9<1,512>)->DenseRange(0,35,1);

template <class S18V, class RS, class SS>
static void BM_successor_s18(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;

	S18V s18(bv);
	RS rs(s18);
	SS ss(s18);
	std::uniform_int_distribution<int> idx(0, SIZE[state.range(0)] - 1);
	for (auto _ : state)
		ss(rs(idx(g)) + 1);

	benchmark::DoNotOptimize(s18.data());
}
BENCHMARK_TEMPLATE(BM_successor_s18, sdsl::s18::vector<1>, sdsl::s18::rank_support<1,1>, sdsl::s18::select_support<1,1>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_s18, sdsl::s18::vector<2>, sdsl::s18::rank_support<1,2>, sdsl::s18::select_support<1,2>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_s18, sdsl::s18::vector<4>, sdsl::s18::rank_support<1,4>, sdsl::s18::select_support<1,4>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_s18, sdsl::s18::vector<8>, sdsl::s18::rank_support<1,8>, sdsl::s18::select_support<1,8>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_s18, sdsl::s18::vector<16>, sdsl::s18::rank_support<1,16>, sdsl::s18::select_support<1,16>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_s18, sdsl::s18::vector<32>, sdsl::s18::rank_support<1,32>, sdsl::s18::select_support<1,32>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_s18, sdsl::s18::vector<64>, sdsl::s18::rank_support<1,64>, sdsl::s18::select_support<1,64>)->DenseRange(0,35,1);

template <class RRR, class RS, class SS>
static void BM_successor_rrr(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;

	RRR rrr(bv);
	RS rs(&rrr);
	SS ss(&rrr);
	std::uniform_int_distribution<int> idx(0, SIZE[state.range(0)] - 1);
	for (auto _ : state)
		ss(rs(idx(g)) + 1);

	benchmark::DoNotOptimize(rrr.begin());
}
BENCHMARK_TEMPLATE(BM_successor_rrr, sdsl::rrr_vector<8>, sdsl::rank_support_rrr<1,8>, sdsl::select_support_rrr<1,8>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_rrr, sdsl::rrr_vector<16>, sdsl::rank_support_rrr<1,16>, sdsl::select_support_rrr<1,16>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_rrr, sdsl::rrr_vector<32>, sdsl::rank_support_rrr<1,32>, sdsl::select_support_rrr<1,32>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_rrr, sdsl::rrr_vector<64>, sdsl::rank_support_rrr<1,64>, sdsl::select_support_rrr<1,64>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_rrr, sdsl::rrr_vector<128>, sdsl::rank_support_rrr<1,128>, sdsl::select_support_rrr<1,128>)->DenseRange(0,35,1);
BENCHMARK_TEMPLATE(BM_successor_rrr, sdsl::rrr_vector<256>, sdsl::rank_support_rrr<1,256>, sdsl::select_support_rrr<1,256>)->DenseRange(0,35,1);

template <class SD, class RS, class SS>
static void BM_successor_sd(benchmark::State& state) {
	sdsl::bit_vector &bv = test_bv(static_cast<int>(state.range(0)));
	std::random_device g;

	SD sd(bv);
	RS rs(&sd);
	SS ss(&sd);
	std::uniform_int_distribution<int> idx(0, SIZE[state.range(0)] - 1);
	for (auto _ : state)
		ss(rs(idx(g)) + 1);

	benchmark::DoNotOptimize(sd.begin());
}
BENCHMARK_TEMPLATE(BM_successor_sd, sdsl::sd_vector<>, sdsl::rank_support_sd<1>, sdsl::select_support_sd<1>)->DenseRange(0,35,1);



BENCHMARK_MAIN();

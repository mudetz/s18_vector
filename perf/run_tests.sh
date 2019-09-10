# !/usr/bin/env bash

sudo cpupower frequency-set --governor performance

./build/perf-suite --benchmark_filter='BM_access.*/0'
./build/perf-suite --benchmark_filter='BM_access.*/1'
./build/perf-suite --benchmark_filter='BM_access.*/2'

./build/perf-suite --benchmark_filter='BM_rank.*/0'
./build/perf-suite --benchmark_filter='BM_rank.*/1'
./build/perf-suite --benchmark_filter='BM_rank.*/2'

./build/perf-suite --benchmark_filter='BM_select.*/0'
./build/perf-suite --benchmark_filter='BM_select.*/1'
./build/perf-suite --benchmark_filter='BM_select.*/2'

sudo cpupower frequency-set --governor powersave

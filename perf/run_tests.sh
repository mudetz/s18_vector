# !/usr/bin/env bash

LAMBDA=127
X=100
P=(0.05 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 0.95)

for i in {0..10}
do
  echo "i=${i},  LAMBDA=${LAMBDA},  X=${X},  P=${P[i]}"
  ./build/perf-suite --benchmark_filter="BM_access.*/$i$" | tee -a "rp${i}.txt"
  ./build/perf-suite --benchmark_filter="BM_rank.*/$i$" | tee -a "rp${i}.txt"
  ./build/perf-suite --benchmark_filter="BM_select.*/$i$" | tee -a "rp${i}.txt"
  ./build/perf-suite --benchmark_filter="BM_successor.*/$i$" | tee -a "rp${i}.txt"
done

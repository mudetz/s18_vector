# !/usr/bin/env bash

LAMBDA=(4 7 31 127)
X=(1000 10000 100000)
P=(0.01 0.001 0.0001)

for i in {13..35}
do
  echo "i=${i},  LAMBDA=${LAMBDA[(i/9)]},  X=${X[((i%9)/3)]},  P=${P[(i%3)]}"

  ./build/perf-suite --benchmark_filter="BM_access.*/$i$"
  ./build/perf-suite --benchmark_filter="BM_rank.*/$i$"
  ./build/perf-suite --benchmark_filter="BM_select.*/$i$"
  ./build/perf-suite --benchmark_filter="BM_successor.*/$i$"
done

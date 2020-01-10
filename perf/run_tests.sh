# !/usr/bin/env bash

LAMBDA=127
X=100
P=(.01 .02 .03 .04 .05 .1 .2 .3 .4 .5 .6 .7 .8 .9 .95)

for i in {0..14}
do
  echo "i=${i},  LAMBDA=${LAMBDA},  X=${X},  P=${P[i]}"
  ./build/perf-suite --benchmark_filter="BM_access.*/$i$" | tee -a "ru${i}.txt"
  ./build/perf-suite --benchmark_filter="BM_rank.*/$i$" | tee -a "ru${i}.txt"
  ./build/perf-suite --benchmark_filter="BM_select.*/$i$" | tee -a "ru${i}.txt"
  ./build/perf-suite --benchmark_filter="BM_successor.*/$i$" | tee -a "ru${i}.txt"
done

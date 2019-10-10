#!/usr/bin/env bash
git clone --depth=1 https://github.com/simongog/sdsl-lite sdsl/src
./sdsl/src/install.sh sdsl/build

git clone --depth=1 https://github.com/google/benchmark.git benchmark
git clone --depth=1 https://github.com/google/googletest.git benchmark/googletest
mkdir benchmark/build/
cd benchmark/build/
cmake -DCMAKE_BUILD_TYPE=Release ../
make -j12
make test
cd -

git clone --depth=1 https://github.com/nixaraven/s9_vector s9


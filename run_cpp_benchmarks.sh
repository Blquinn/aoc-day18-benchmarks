#!/bin/bash

set -e

mkdir -p cpp/build
cd cpp/build
meson .. &>/dev/null
meson configure -Dbuildtype=release -Ddebug=false -Doptimization=3 &>/dev/null

set_impls=('rbtree' 'ankerl_dense_set' 'btree' 'hashset' 'absl_flat_set')

for set_i in ${set_impls[@]}; do
  echo "Running benchmark for set implementation $set_i"
  meson configure -Dset_impl=$set_i &>/dev/null
  ninja &>/dev/null
  ./bench
  echo ''
  echo ''
done

set +e

cd ../../

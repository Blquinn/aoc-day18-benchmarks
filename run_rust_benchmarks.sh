#!/bin/bash

set -e

cd rust

set_impls=('btree' 'hashset' 'fxhash')

for set_i in ${set_impls[@]}; do
  echo "Running benchmark for set implementation $set_i"

  if [ $set_i == 'btree' ]; then
    cargo bench
  else
    cargo bench --features $set_i
  fi

  echo ''
  echo ''
done

set +e

cd ../../

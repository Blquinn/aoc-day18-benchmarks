# AOC Day18 Rust vs CPP Benchmarks

### Rust benchmarks (BTree Set)

Ran using `cargo bench` on rust version:

```
nightly-x86_64-unknown-linux-gnu (default)
rustc 1.68.0-nightly (3984bc583 2023-01-17)
```

Results

Run | Avg Time
---|---
1 | 32046ns 
2 | 33703ns 
3 | 31932ns 

### C++ Benchmarks

Clang version: `clang 15.0.4 "clang version 15.0.4 (Fedora 15.0.4-1.fc37)")`
Gcc version: `(gcc 12.2.1 "c++ (GCC) 12.2.1 20221121 (Red Hat 12.2.1-4)"`
Optimization level: `3`

Results

For each test I ran the bench 3 times and took the best time.

Note that the ankerl implementation is not actually returning the correct results.

Compiler | Set Impl | Avg Time
---|---|---
Clang | RBTree | 29575ns
GCC | RBTree | 28443ns
Clang | std::unordered_set | 23065ns
GCC | std::unordered_set | 23478ns
Clang | ankerl::unordered_dense::set | 18022ns
GCC | ankerl::unordered_dense::set | 17663ns
Clang | absl::flat_hash_set | 22292ns
GCC | absl::flat_hash_set | 29954ns
Clang | absl::btree_set | 36101ns
GCC | absl::btree_set | 46261ns

#![feature(never_type)]
#![feature(test)]

use std::{
    collections::BTreeSet,
    fmt,
    fs::File,
    io::{BufWriter, Write},
    str::FromStr,
};

use ordered_float::NotNan;

#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, derive_more::Add, Hash)]
struct Cube {
    x: i32,
    y: i32,
    z: i32,
}

impl fmt::Debug for Cube {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let Cube { x, y, z } = self;
        write!(f, "{x},{y},{z}")
    }
}

impl FromStr for Cube {
    type Err = !;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut tokens = s.split(',').map(|s| s.parse::<i32>().unwrap());
        Ok(Self {
            x: tokens.next().unwrap(),
            y: tokens.next().unwrap(),
            z: tokens.next().unwrap(),
        })
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, derive_more::Add, Hash)]
struct Side {
    // This is so that we can derive `Ord`, `Eq` and `Hash`. It means a bunch of
    // `.try_into().unwrap()` and `.into_inner()` later, but it's the price to
    // pay to be able to use it as a BTreeSet/HashSet key.
    x: NotNan<f32>,
    y: NotNan<f32>,
    z: NotNan<f32>,
}

#[derive(Default)]
struct State {
    cubes: BTreeSet<Cube>,
    sides: BTreeSet<Side>,

    min_x: i32,
    max_x: i32,

    min_y: i32,
    max_y: i32,

    min_z: i32,
    max_z: i32,

    lava_cubes: BTreeSet<Cube>,
}

impl State {
    fn get_opposite_cube(&self, side: Side) -> Cube {
        let diffs = [-0.5_f32, 0.5];
        // instead of casting float->int->float, we just do truncation
        let is_x = side.x != side.x.trunc();
        let is_y = side.y != side.y.trunc();
        let is_z = side.z != side.z.trunc();

        for diff in diffs {
            let cube = Cube {
                // we might be able to cast a `bool` to a floating point like
                // they do in the C/C++ version, but I refuse.
                x: (side.x + if is_x { diff } else { 0.0 }).into_inner() as _,
                y: (side.y + if is_y { diff } else { 0.0 }).into_inner() as _,
                z: (side.z + if is_z { diff } else { 0.0 }).into_inner() as _,
            };
            if self.cubes.contains(&cube) {
                return Cube {
                    x: (side.x + if is_x { -diff } else { 0.0 }).into_inner() as _,
                    y: (side.y + if is_y { -diff } else { 0.0 }).into_inner() as _,
                    z: (side.z + if is_z { -diff } else { 0.0 }).into_inner() as _,
                };
            }
        }

        panic!("Not found") // best error msg eva <333
    }

    fn is_lava_side(&self, side: Side) -> bool {
        self.lava_cubes.contains(&self.get_opposite_cube(side))
    }

    fn dfs_lava(&mut self, lava: Cube) {
        // Off limits
        if (lava.x > self.max_x + 1 || lava.x < self.min_x - 1)
            || (lava.y > self.max_y + 1 || lava.y < self.min_y - 1)
            || (lava.z > self.max_z + 1 || lava.z < self.min_z - 1)
        {
            return;
        }

        // Can't be both lava and non-lava
        if self.cubes.contains(&lava) {
            return;
        }

        // Already inserted
        if !self.lava_cubes.insert(lava) {
            return;
        }

        // Start dfs
        let differs = [
            // that part's shorter in C++, because struct initializers don't
            // need to be named: you put the whole type on the left-hand side
            // and bracket soup on the right-hand side and weeeee
            Cube { x: -1, y: 0, z: 0 },
            Cube { x: 1, y: 0, z: 0 },
            Cube { x: 0, y: -1, z: 0 },
            Cube { x: 0, y: 1, z: 0 },
            Cube { x: 0, y: 0, z: -1 },
            Cube { x: 0, y: 0, z: 1 },
        ];

        for differ in differs {
            // this is what you'd put in `stacker` if you needed it (see below)
            self.dfs_lava(lava + differ);
        }
    }

    // Mark the corner as a lava cube
    // DFS from there
    fn populate_lava_cubes(&mut self) {
        let cube = self.cubes.first().unwrap();

        // I tried using `RangeInclusive` for this but had off-by-one errors,
        // so I made the port more direct and used 6 separate values instead.

        self.min_x = cube.x;
        self.max_x = cube.x;

        self.min_y = cube.y;
        self.max_y = cube.y;

        self.min_z = cube.z;
        self.max_z = cube.z;

        for cube in &self.cubes {
            // you might, like me, be tempted to reach for `minmax` from
            // itertools, and then later realize you don't need the "minmax 3d
            // coordinate", you need the minmax x, minmax y and minmax z,
            // separately, and that's a different operation.
            self.min_x = self.min_x.min(cube.x);
            self.max_x = self.max_x.max(cube.x);

            self.min_y = self.min_y.min(cube.y);
            self.max_y = self.max_y.max(cube.y);

            self.min_z = self.min_z.min(cube.z);
            self.max_z = self.max_z.max(cube.z);
        }

        self.dfs_lava(Cube {
            x: self.min_x - 1,
            y: self.min_y - 1,
            z: self.min_z - 1,
        });
    }

    fn filter_sides(&mut self) {
        self.populate_lava_cubes();
        assert!(!self.lava_cubes.is_empty());

        let mut sides = std::mem::take(&mut self.sides);
        sides.retain(|side| {
            // in C/C++ these are just `assert!(something_coerced_to_int_or_bool)`
            assert!(side.x.into_inner() > 0.0);
            assert!(side.y.into_inner() > 0.0);
            assert!(side.z.into_inner() > 0.0);

            let a = Cube {
                // escaping our `NotNan` prison here. Maybe we should be
                // using `.try_into()` instead, so it panics on `NaN` /
                // too-large numbers?
                x: side.x.into_inner() as i32,
                y: side.y.into_inner() as i32,
                z: side.z.into_inner() as i32,
            };
            let mut b = a;

            if side.x != side.x.trunc() {
                b.x += 1;
            } else if side.y != side.y.trunc() {
                b.y += 1;
            } else if side.z != side.z.trunc() {
                b.z += 1;
            } else {
                panic!("Invalid side");
            }

            let has_a = self.cubes.contains(&a);
            let has_b = self.cubes.contains(&b);

            // Can't both be false
            assert!(
                has_a || has_b,
                "invalid side: {side:?}, has_a={has_a}, has_b={has_b}"
            );

            // Not exposed to air
            if has_a && has_b {
                return false;
            }

            // Not exposed to lava
            if !self.is_lava_side(*side) {
                return false;
            }

            true
        });
        self.sides = sides;
    }

    fn get_surface_area(&mut self) -> usize {
        for cube in &self.cubes {
            let diffs = [-0.5_f32, 0.5];
            for diff in diffs {
                // this is ugly and I don't like it. there's better ways to
                // write this that don't even include macros, but too late, I've
                // already moved on.
                self.sides.insert(Side {
                    x: (cube.x as f32 + diff).try_into().unwrap(),
                    y: (cube.y as f32).try_into().unwrap(),
                    z: (cube.z as f32).try_into().unwrap(),
                });
                self.sides.insert(Side {
                    x: (cube.x as f32).try_into().unwrap(),
                    y: (cube.y as f32 + diff).try_into().unwrap(),
                    z: (cube.z as f32).try_into().unwrap(),
                });
                self.sides.insert(Side {
                    x: (cube.x as f32).try_into().unwrap(),
                    y: (cube.y as f32).try_into().unwrap(),
                    z: (cube.z as f32 + diff).try_into().unwrap(),
                });
            }
        }

        self.filter_sides();
        self.sides.len()
    }
}

pub fn run() -> usize {
    let unit_cube = Cube { x: 1, y: 1, z: 1 };

    let cubes = include_str!("input.txt")
        .lines()
        .map(|l| Cube::from_str(l).unwrap() + unit_cube)
        .collect::<_>();
    let mut state = State {
        cubes,
        ..Default::default()
    };

    // let mut f = BufWriter::new(File::create("PARSE").unwrap());
    // for c in &state.cubes {
    //     writeln!(f, "{c:?}").unwrap();
    // }
    // f.flush().unwrap();

    return state.get_surface_area();
}

mod benchmarks {
    extern crate test;

    #[bench]
    fn bench_run(b: &mut test::Bencher) {
        b.iter(|| {
            let res = crate::run();
            assert!(res == 58);
        })
    }
}

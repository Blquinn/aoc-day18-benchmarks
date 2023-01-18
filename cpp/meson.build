project('aoc', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'])

deps = [
  dependency('benchmark', required: true),
  dependency('absl_base', required: true),
  dependency('absl_flags', required: true),
  dependency('absl_hash', required: true),
  dependency('absl_container_common', required: true),
  dependency('absl_container_memory', required: true),
]
 
executable('bench',
  ['bench.cpp'],
  dependencies: deps,
  install : true)

executable('part2',
  ['main-part2.cpp'],
  dependencies: deps,
  install : true)

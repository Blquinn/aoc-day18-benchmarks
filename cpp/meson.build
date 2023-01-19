project('aoc', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'])

cpp_flags = ''

if get_option('set_impl') == 'btree'
  cpp_flags = '-DUSE_BTREE=1'
elif get_option('set_impl') == 'hashset'
  cpp_flags = '-DUSE_HASHSET=1'
elif get_option('set_impl') == 'absl_flat_set'
  cpp_flags = '-DUSE_ABSL_FLAT_SET=1'
elif get_option('set_impl') == 'ankerl_dense_set'
  cpp_flags = '-DUSE_ANKRL=1'
endif

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
  install : true, 
  cpp_args : cpp_flags)

executable('part2',
  ['main-part2.cpp'],
  dependencies: deps,
  install : true, 
  cpp_args: cpp_flags)

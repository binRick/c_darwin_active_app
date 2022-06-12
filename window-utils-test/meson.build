window_utils_test_srcs = [
  'window-utils-test.c',
]
window_utils_test_include_dirs = [
       inc,
       '../window-utils',
       '../app-utils',
]
window_utils_test_deps = [
  greatest_dep,
  c_stringfn_dep,
  app_utils_dep,
  window_utils_dep,
  logh_dep,
]

if get_option('enable-binaries')
  window_utils_test_exec = executable('window-utils-test',
     window_utils_test_srcs,
     dependencies: window_utils_test_deps,
     include_directories: window_utils_test_include_dirs,
  )
  if get_option('enable-binary-tests')
    test('window-utils-test', window_utils_test_exec, args: [])
  endif
endif

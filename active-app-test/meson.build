active_app_test_srcs = [
  'active-app-test.c',
]
active_app_include_dirs = [
]
active_app_test_deps = [
  greatest_dep,
  c_stringfn_dep,
  active_app_dep,
]

if get_option('enable-binaries')
  active_app_test_exec = executable('active-app-test',
     [
      active_app_test_srcs,
     ],
     dependencies: active_app_test_deps,
     include_directories: [
       active_app_include_dirs,
       inc,
     ],
  )
  if get_option('enable-binary-tests')
    test('active-app-test', active_app_test_exec, args: ['--test'])
  endif
endif

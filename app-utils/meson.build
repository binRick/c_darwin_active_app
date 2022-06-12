app_utils_srcs = [
  'app-utils.c',
]
app_utils_deps = [
  c_stringfn_dep,
]
app_utils_lib = static_library('app-utils',
  app_utils_srcs,
  install: false,
  dependencies: app_utils_deps,
  include_directories: [
    inc,
  ],
)


app_utils_dep = declare_dependency(
  include_directories: [
    inc,
  ],
  link_with: app_utils_lib,
)

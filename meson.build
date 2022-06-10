project('c_darwin_active_app', ['c'],
  default_options: [
   'warning_level=0',
   'c_std=gnu99',
  ]
)
bins = [
  'm1',
]
config = configuration_data()
version = run_command('git', 'describe', '--tags')
config.set('version', version.stdout().strip())
add_project_arguments('-D_GNU_SOURCE', language : 'c')
add_project_arguments('-DDBG_H_DEF_ONCE', language : 'c')
add_project_arguments('-DDBG_H_NO_WARNING', language : 'c')
add_project_arguments('-DASSERTF_DEF_ONCE', language : 'c')
add_project_arguments('-DLOG_USE_COLOR', language : 'c')
add_project_arguments('-DLOG_LEVEL=TRACE', language : 'c')
add_project_arguments('-Wno-pragma-once-outside-header', language : 'c')
add_project_arguments('-Wno-incompatible-pointer-types', language : 'c')
add_project_arguments('-Wno-pointer-sign', language : 'c')
add_project_arguments('-Wno-unused-value', language : 'c')
add_project_arguments('-Wno-gnu-alignof-expression', language : 'c')
add_project_arguments('-Wno-varargs', language : 'c')
add_project_arguments('-Wno-return-type', language : 'c')
#//***********************************************************/
deps = []
bin_srcs = []
submodule_srcs = []
cc = meson.get_compiler('c')
#//***********************************************************/
inc = include_directories(
  'submodules/',
)
#//***********************************************************/
cargs_dep = dependency('cargs',fallback:['cargs', 'cargs_dep'])
parson_shared_dep = dependency('parson',fallback:['parson', 'parson_shared_dep'])
#//////////////////////////////////////////////////////////////////////
subdir('active-app')
subdir('active-app-test')
#//////////////////////////////////////////////////////////////////////

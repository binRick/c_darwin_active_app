project('c_darwin_active_app', ['c'],default_options: ['warning_level=0','c_std=gnu99',])
cc = meson.get_compiler('c')
pwd = run_command(find_program('pwd', required : true), check: true).stdout().strip()
meson_deps_dir = pwd+'/submodules/meson_deps'
inc = [include_directories(
  './',
  './app-utils',
  './window-utils',
)]
#############################################################################
subdir('submodules/meson_deps/meson/osx')
subdir('submodules/meson_deps/meson/args')
subdir('submodules/meson_deps/meson/inc')
#//***********************************************************/
subdir('submodules/meson_deps/meson/deps/m')
subdir('submodules/meson_deps/meson/deps/cargs')
subdir('submodules/meson_deps/meson/deps/c_string_buffer')
subdir('submodules/meson_deps/meson/deps/c_stringfn')
subdir('submodules/meson_deps/meson/deps/c_fsio')
subdir('submodules/meson_deps/meson/deps/djbhash')
subdir('submodules/meson_deps/meson/deps/parson')
subdir('submodules/meson_deps/meson/deps/b64')
subdir('submodules/meson_deps/meson/deps/bytes')
subdir('submodules/meson_deps/meson/deps/greatest')
subdir('submodules/meson_deps/meson/deps/ansicodes')
subdir('submodules/meson_deps/meson/deps/log.h')
#//////////////////////////////////////////////////////////////////////
subdir('active-app')
subdir('app-utils')
subdir('window-utils')
subdir('active-app-module')
#//////////////////////////////////////////////////////////////////////
subdir('app-utils-test')
subdir('window-utils-test')
subdir('active-app-test')
subdir('active-app-module-test')
#//////////////////////////////////////////////////////////////////////

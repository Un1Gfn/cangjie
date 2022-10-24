project('cj5.cgi', 'c',
  default_options: [
    'bindir=cgi-bin',
    'backend=ninja',
    'buildtype=debug',
    'debug=true',
    'strip=false',
    'warning_level=3',
    'werror=true',
    'b_lto=true',
    'b_ndebug=false',
    'c_std=gnu17',
  ],
  license: 'AGPL-3.0',
  meson_version: '>=0.62.1', # postmarketOS v22.06
  version: '0.1',
)

add_global_arguments(
  '-D_GNU_SOURCE',
  '-Wdeprecated-declarations',
  '-Winline',
  '-Wno-pedantic',
  '-Wshadow',
  '-Wno-error=deprecated-declarations',
  '-Wno-error=unused-but-set-variable',
  '-Wno-error=unused-function',
  '-Wno-error=unused-label',
  '-Wno-error=unused-parameter',
  '-Wno-error=unused-variable',
  language: 'c'
)

CC=meson.get_compiler('c')

executable(meson.project_name(),
  'main.c',
  dependencies: [
    # (run_command('uname', '-m').stdout()=='x86_64' ? dependency('kcgi')      : CC.find_library('kcgi',     required : true)),
    # (run_command('uname', '-m').stdout()=='x86_64' ? dependency('kcgi-html') : CC.find_library('kcgihtml', required : true)),
    dependency('kcgi'),
    dependency('kcgi-html'),
    CC.find_library('gdbm', required : true), # https://github.com/mesonbuild/meson/issues/4297
  ],
  install: true,
)

install_data('cj5.css', install_dir: get_option('prefix'))
install_data('../app.cj5.mkdict/Cangjie5.gdbm', install_dir: get_option('prefix'))
install_data('../app.cj5.mkdict/Cangjie5_special.gdbm', install_dir: get_option('prefix'))

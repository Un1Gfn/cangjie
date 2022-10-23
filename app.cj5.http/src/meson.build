project('cj5.cgi', 'c',
  default_options: [
    'prefix=/home/darren/cangjie/app.cj5.http/srv/',
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
  meson_version: '>=0.63.3',
  version: '0.1',
)

add_global_arguments('-D_GNU_SOURCE',
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
                     language: 'c')

executable(meson.project_name(),
  'main.c',
  # '/home/darren/cangjie/app.cj5.lookup/l2r.c',
  dependencies: [
    dependency('kcgi'),
    dependency('kcgi-html'),
    # https://github.com/mesonbuild/meson/issues/4297
    meson.get_compiler('c').find_library('gdbm', required : true)
  ],
  install: true,
)

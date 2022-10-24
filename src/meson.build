project('cangjie', 'c',
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
    'c_std=c17',
  ],
  license: 'AGPL-3.0',
  meson_version: '>=0.62.1', # postmarketOS v22.06
  version: '0.1',
)

add_global_arguments(
  '-D_GNU_SOURCE', # musl compatibility issue
  '-Wdeprecated-declarations',
  '-Winline',
  '-Wno-pedantic',
  '-Wshadow',
  '-fanalyzer',
  '-fextended-identifiers', # \unnnn and \U00nnnnnn notations
  '-Wno-error=deprecated-declarations',
  '-Wno-error=unused-but-set-variable',
  '-Wno-error=unused-function',
  '-Wno-error=unused-label',
  '-Wno-error=unused-parameter',
  '-Wno-error=unused-variable',
  language: 'c'
)

# subdir('include')
# subdir('bin.aux')
subdir('bin.mkdict')
# subdir('bin.lookup')
subdir('bin.http')

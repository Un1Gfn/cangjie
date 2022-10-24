executable('cj5.cgi',
  'main.c',
  dependencies: [
    # (run_command('uname', '-m').stdout()=='x86_64' ? dependency('kcgi')      : CC.find_library('kcgi',     required : true)),
    # (run_command('uname', '-m').stdout()=='x86_64' ? dependency('kcgi-html') : CC.find_library('kcgihtml', required : true)),
    dependency('kcgi'),
    dependency('kcgi-html'),
    meson.get_compiler('c').find_library('gdbm', required: true), # https://github.com/mesonbuild/meson/issues/4297
  ],
  install: true,
)

install_data('cj5.css', install_dir: get_option('prefix'))

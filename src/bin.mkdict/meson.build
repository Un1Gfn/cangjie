E=executable('mkdict.out',
  'mkdict.c',
  dependencies: [meson.get_compiler('c').find_library('gdbm', required: true),],
  install: false,
)

# install_data('../app.cj5.mkdict/Cangjie5.gdbm', install_dir: get_option('prefix'))

d_sp=custom_target('d_sp',
  input: ['cc.sh', E, 'Cangjie5_special.txt',],
  output: 'Cangjie5_special.gdbm',
  command: ['bash', '-e', '-x', '@INPUT0@', 'modeA', '@INPUT1@', '@INPUT2@', '@OUTPUT@' ],
  install: true,
  install_dir: get_option('prefix'),
)

# d_nosp=custom_target('d_nosp',
#   input: ['cc.sh', E, 'Cangjie5.txt',],
#   output: 'Cangjie5.gdbm',
#   command: ['bash', '-e', '-x', '@INPUT0@', 'modeB', '@INPUT1@', '@INPUT2@', '@OUTPUT@' ],
#   install: true,
#   install_dir: get_option('prefix'),
# )

check_dep = dependency('check')
cc = meson.get_compiler('c')

tests_list = [
  [ 'testparse.c', '../src/parse.c']
]


t_defines = [
  '-DTEST_FILE_PATH="' + meson.current_source_dir() + '/files"',
]

foreach t : tests_list

  file_name = t[0]
  test_name = file_name.split('.')[0]

  link_file = t.get(1, [])
  end  = link_file.split('/')[-1]
  name = end.split('.')[0]

  # Log file needs to be linked always
  src = [link_file, '../src/log.c']

  ld_link = library(name, src)

  e = executable(test_name, 
          file_name, 
          link_with : ld_link,
          dependencies : check_dep,
          c_args : t_defines)
  test('Test wave parsing', e)
endforeach

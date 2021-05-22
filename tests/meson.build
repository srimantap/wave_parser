check_dep = dependency('check')
cc = meson.get_compiler('c')

tests_list = [
  [ 'testparse.c', '../src/parse.c']
]



foreach t : tests_list

  file_name = t[0]
  test_name = file_name.split('.')[0]

  link_file = t.get(1, [])
  end  = link_file.split('/')[-1]
  name = end.split('.')[0]

  ld_link = library(name, link_file)

  e = executable(test_name, 
          file_name, 
          link_with : ld_link,
          dependencies : check_dep)
  test('Test wave parsing', e)
endforeach

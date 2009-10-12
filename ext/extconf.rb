require 'mkmf'

have_library('pcre', 'pcre_compile')
create_makefile("version_sorter")

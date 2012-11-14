require 'mkmf'

$defs.push("-DBUILD_FOR_RUBY")
have_library('pcre', 'pcre_compile')
create_makefile("version_sorter")

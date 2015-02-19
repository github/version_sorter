# encoding: utf-8
require 'rbconfig'

Gem::Specification.new do |s|
  s.name     = 'version_sorter'
  s.version  = '2.0.0'
  s.authors  = ["Chris Wanstrath", "K. Adam Christensen"]
  s.email    = 'chris@ozmm.org'
  s.homepage = 'https://github.com/defunkt/version_sorter'
  s.license  = 'MIT'

  s.summary     = "Fast sorting of version strings"
  s.description = "VersionSorter is a C extension that does fast sorting of large sets of version strings."

  s.files = Dir['{lib,ext}/**/*', 'LICENSE*']
  s.extensions << 'ext/version_sorter/extconf.rb'

  # include only files in version control
  git_dir = File.expand_path('../.git', __FILE__)
  dev_null = defined?(File::NULL) ? File::NULL :
    RbConfig::CONFIG['host_os'] =~ /msdos|mswin|djgpp|mingw/ ? 'NUL' : '/dev/null'
  git_files = `git --git-dir='#{git_dir}' ls-files -z 2>#{dev_null}`.split("\0")
  s.files &= git_files if git_files.any?
end

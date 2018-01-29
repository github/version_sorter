# encoding: utf-8
require 'rbconfig'

Gem::Specification.new do |s|
  s.name     = 'version_sorter'
  s.version  = '2.2.1'
  s.authors  = ["Chris Wanstrath", "K. Adam Christensen"]
  s.email    = 'chris@ozmm.org'
  s.homepage = 'https://github.com/github/version_sorter#readme'
  s.license  = 'MIT'

  s.summary     = "Fast sorting of version strings"
  s.description = "VersionSorter is a C extension that does fast sorting of large sets of version strings."

  s.files = Dir['lib/**/*', 'LICENSE*', 'Cargo.*', 'src/**/*']

  s.platform = Gem::Platform::CURRENT

  s.add_development_dependency 'helix_runtime', '~> 0.7'
  s.add_runtime_dependency 'helix_runtime', '~> 0.7'
end

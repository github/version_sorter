require 'rake/testtask'

task :default => [ :compile, :test ]

Rake::TestTask.new  do |t|
  t.libs << 'test'
  t.test_files = FileList['test/*test.rb']
end

begin
  require 'rake/extensiontask'
  Rake::ExtensionTask.new('version_sorter')
  task :test => :compile
rescue LoadError
  puts 'The rake-compiler gem is required'
end

begin
  require 'jeweler'
  Jeweler::Tasks.new do |gemspec|
    gemspec.name = "version_sorter"
    gemspec.summary = "Fast sorting of version strings"
    gemspec.description = "Fast sorting of version strings"
    gemspec.email = "chris@ozmm.org"
    gemspec.homepage = "http://github.com/defunkt/version_sorter"
    gemspec.authors = ["Chris Wanstrath", "K. Adam Christensen"]
    require 'lib/version_sorter/version'
    gemspec.version = VersionSorter::Version
    gemspec.require_paths = ["lib", "ext"]
    gemspec.files.include("ext")
    gemspec.extensions << 'ext/version_sorter/extconf.rb'
  end
rescue LoadError
  puts "Jeweler not available. Install it with: "
  puts "gem install jeweler -s http://gemcutter.org"
end

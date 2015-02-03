require 'rake/testtask'
require 'rake/extensiontask'

task :default => :test

Rake::TestTask.new(:test) do |t|
  t.libs << 'test'
  t.test_files = FileList['test/*test.rb']
end

Rake::ExtensionTask.new('version_sorter')
task :test => :compile

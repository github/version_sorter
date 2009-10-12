require 'rake/extensiontask'
require 'rake/testtask'

Rake::ExtensionTask.new('version_sorter')

Rake::TestTask.new  do |t|
  t.libs << 'test'
  t.test_files = FileList['test/*test.rb']
end

task :test => :compile

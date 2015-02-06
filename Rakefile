require 'rake/testtask'
require 'rake/extensiontask'

task :default => :test

Rake::TestTask.new(:test) do |t|
  t.libs << 'test'
  t.test_files = FileList['test/*test.rb']
end

Rake::ExtensionTask.new('version_sorter')

desc "Compile the native extension"
task :test => :compile

desc "Run the benchmark"
task :benchmark => :compile do
  sh 'script/benchmark'
end

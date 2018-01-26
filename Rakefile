require 'rake/testtask'
import 'lib/tasks/helix_runtime.rake'

task :default => :test

Rake::TestTask.new(:test) do |t|
  t.libs << 'test'
  t.test_files = FileList['test/*test.rb']
end

desc "Run the benchmark"
task :benchmark do
  sh 'script/benchmark'
end

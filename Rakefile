require 'rake/testtask'

task :default => :test

Rake::TestTask.new(:test) do |t|
  t.libs << 'test'
  t.test_files = FileList['test/*test.rb']
end

file 'lib/version_sorter.bundle' => FileList['ext/version_sorter/*.{c,h,rb}'] do |task|
  Dir.chdir 'ext/version_sorter' do
    sh 'ruby extconf.rb'
    sh 'make'
  end
  mv 'ext/version_sorter/version_sorter.bundle', task.name
end

desc "Compile the native extension"
task :compile => 'lib/version_sorter.bundle'
task :test => :compile

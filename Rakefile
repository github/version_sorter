require 'rake/testtask'

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

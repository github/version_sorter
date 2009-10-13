require 'test/unit'
$LOAD_PATH.unshift File.dirname(__FILE__) + '/../lib'
require 'version_sorter'

class VersionSorterTest < Test::Unit::TestCase
  include VersionSorter

  def test_sorts_verisons_correctly
    versions = %w(1.0.9 1.0.10 2.0 3.1.4.2 1.0.9a)
    sorted_versions = %w( 1.0.9 1.0.9a 1.0.10 2.0 3.1.4.2 )

    assert_equal sorted_versions, sort(versions)
  end

  def test_reverse_sorts_verisons_correctly
    versions = %w(1.0.9 1.0.10 2.0 3.1.4.2 1.0.9a)
    sorted_versions = %w( 3.1.4.2 2.0 1.0.10 1.0.9a 1.0.9 )

    assert_equal sorted_versions, rsort(versions)
  end
end

require 'benchmark'
versions = IO.read(File.dirname(__FILE__) + '/tags.txt').split("\n")
count = 10
Benchmark.bm(20) do |x|
  x.report("sort")             { count.times { VersionSorter.sort(versions) } }
  x.report("rsot")             { count.times { VersionSorter.rsort(versions) } }
end
puts

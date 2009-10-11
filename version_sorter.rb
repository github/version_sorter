require 'ffi'
require 'pp'

module VersionSorter
  extend FFI::Library

  ffi_lib "ext/version_sorter/build/Debug/libversion_sorter.dylib"
  attach_function :version_sorter_sort, [:pointer, :int], :void

  extend self

  def sort(list)

    strptrs = list.map { |x| FFI::MemoryPointer.from_string(x) }

    argv = FFI::MemoryPointer.new(:pointer, list.size)
    argv.write_array_of_pointer(strptrs)

    version_sorter_sort(argv, list.size)

    argv.read_array_of_pointer(list.size).map { |x| x.get_string(0) }
  end

  def rsort(list)
    sort(list).reverse
  end
end

if $0 == __FILE__
  require 'test/unit'

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
  versions = IO.read('tags.txt').split("\n")
  count = 10
  Benchmark.bm(20) do |x|
    x.report("sort")             { count.times { VersionSorter.sort(versions) } }
    x.report("rsot")             { count.times { VersionSorter.rsort(versions) } }
  end
  puts

end

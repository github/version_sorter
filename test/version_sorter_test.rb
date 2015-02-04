require 'test/unit'
require 'version_sorter'

class VersionSorterTest < Test::Unit::TestCase
  include VersionSorter

  def test_sorts_versions_correctly
    versions = %w(1.0.9 1.0.10 2.0 3.1.4.2 1.0.9a)
    sorted_versions = %w( 1.0.9 1.0.9a 1.0.10 2.0 3.1.4.2 )

    assert_equal sorted_versions, sort(versions)
  end

  def test_returns_same_object
    versions = %w( 2.0 1.0 0.5 )
    sorted = sort(versions)

    assert_equal versions[2].object_id, sorted[0].object_id
  end

  def test_reverse_sorts_versions_correctly
    versions = %w(1.0.9 1.0.10 2.0 3.1.4.2 1.0.9a)
    sorted_versions = %w( 3.1.4.2 2.0 1.0.10 1.0.9a 1.0.9 )

    assert_equal sorted_versions, rsort(versions)
  end
end

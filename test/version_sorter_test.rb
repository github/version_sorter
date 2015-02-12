require 'test/unit'
require 'version_sorter'
require 'rubygems/version'

class VersionSorterTest < Test::Unit::TestCase
  def test_sorts_versions_correctly
    versions = %w(1.0.9 1.0.10 2.0 3.1.4.2 1.0.9a)
    sorted_versions = %w( 1.0.9a 1.0.9 1.0.10 2.0 3.1.4.2 )

    assert_equal sorted_versions, VersionSorter.sort(versions)
  end

  def test_sorts_versions_like_rubygems
    versions = %w(1.0.9.b 1.0.9 1.0.10 2.0 3.1.4.2 1.0.9a 2.0rc2 2.0-rc1)
    sorted_versions = versions.sort_by { |v| Gem::Version.new(v) }

    assert_equal sorted_versions, VersionSorter.sort(versions)
  end

  def test_returns_same_object
    versions = %w( 2.0 1.0 0.5 )
    sorted = VersionSorter.sort(versions)

    assert_equal versions[2].object_id, sorted[0].object_id
  end

  def test_reverse_sorts_versions_correctly
    versions = %w(1.0.9 1.0.10 2.0 3.1.4.2 1.0.9a)
    sorted_versions = %w( 3.1.4.2 2.0 1.0.10 1.0.9 1.0.9a )

    assert_equal sorted_versions, VersionSorter.rsort(versions)
  end

  def test_does_not_raise_on_number_overflow
    big_numbers = [
      (2**32).to_s,
      (2**32 + 1).to_s,
      (2**32 + 2).to_s,
      (2**32 - 2).to_s,
      (2**32 - 1).to_s,
    ]
    randomized = big_numbers.sample(big_numbers.size)

    assert_equal big_numbers, VersionSorter.sort(randomized)
  end
end

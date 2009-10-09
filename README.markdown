VersionSorter
=============

This is not code I wrote. Just code I use.

    require 'version_sorter'
    versions = %w( 1.0.9 2.0 1.0.10 1.0.3 )
    VersionSorter.rsort(versions) # => ["2.0", "1.0.10", "1.0.9", "1.0.3"]
    VersionSorter.sort(versions)  # => ["1.0.3", "1.0.9", "1.0.10", "2.0"]


Speed
-----

Want to make it faster?

    require 'tags'
    VersionSorter.sort(@tags)

The `@tags` array is YUI's tags, which they create on build.

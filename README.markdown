VersionSorter
=============

By pope.

    require 'version_sorter'
    versions = %w( 1.0.9 2.0 1.0.10 1.0.3 )
    VersionSorter.rsort(versions) # => ["2.0", "1.0.10", "1.0.9", "1.0.3"]
    VersionSorter.sort(versions)  # => ["1.0.3", "1.0.9", "1.0.10", "2.0"]

<http://github.com/blog/521-speedy-version-sorting>

Install
-------

## [Gemcutter](http://gemcutter.org)

    $ gem install version_sorter

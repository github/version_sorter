# Version sorter

Fast sorting of strings representing version numbers.

```rb
require 'version_sorter'

versions = ["1.0.9", "2.0", "1.0.10", "1.0.3", "2.0.pre"]

VersionSorter.sort(versions)
#=> 1.0.3
#=> 1.0.9
#=> 1.0.10
#=> 2.0.pre
#=> 2.0
```

You can also sort arrays of arbitrary objects by providing a block.

```rb
VersionSorter.sort(tags) { |tag| tag.name }
```

Library API:

```rb
VersionSorter.sort(versions)   #=> sorted array
VersionSorter.rsort(versions)  #=> reverse sorted array

VersionSorter.sort!(versions)  # sort array in place
VersionSorter.rsort!(versions) # reverse sort array in place

VersionSorter.compare(version_a, version_b) #=> positive or negative number depending on which way to sort 
```

<http://github.com/blog/521-speedy-version-sorting>
